/*********************************************************************************************************************************
  Modem_UBLOX_Generic.h
  
  For ESP8266, ESP32, SAMD21/SAMD51, nRF52, SAM DUE, Teensy and STM32 with GSM modules
  
  GSM_Generic is a library for the ESP8266, ESP32, SAMD21/SAMD51, nRF52, SAM DUE, Teensy and STM32 with GSM modules
  
  Based on and modified from MKRGSM Library (https://github.com/arduino-libraries/MKRGSM)
  
  Built by Khoi Hoang https://github.com/khoih-prog/GSM_Generic
  Licensed under GNU Lesser General Public License
  
  Copyright (C) 2017  Arduino AG (http://www.arduino.cc/)
  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License 
  as published by the Free Software Foundation, either version 2.1 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with this program.
  If not, see <https://www.gnu.org/licenses/>.  
 
  Version: 1.6.1
  
  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.2.4    K Hoang     11/03/2021 Initial public release to add support to many boards / modules besides MKRGSM 1400 / SARA U201
  1.3.0    K Hoang     31/03/2021 Add ThingStream MQTTS support. Fix SMS receive bug.
  1.3.1    K Hoang     25/04/2021 Fix bug making ESP32 reset repeatedly.
  1.4.0    K Hoang     28/06/2021 Add support to RP2040-based boards using Arduino mbed or Arduino-pico core
  1.5.0    K Hoang     14/08/2021 Add support to Adafruit nRF52 core v0.22.0+
  1.6.0    K Hoang     31/08/2021 Add support to Ublox LARA-R2xx LTE Cat 1 Modem
  1.6.1    K Hoang     31/08/2021 Fix GPRS issue for Ublox LARA-R2xx LTE Cat 1 Modem
 **********************************************************************************************************************************/

#pragma once

#ifndef _MODEM_UBLOX_INCLUDED_H
#define _MODEM_UBLOX_INCLUDED_H

#include <stdarg.h>
#include <stdio.h>

#include <Arduino.h>
#include <IPAddress.h>

#include "GSM_Generic_Debug.h"

#include "GSM_Type_Generic.h"

#include "GSM_Boards_Generic.h"

#if ( GSM_USE_SAMD || GSM_USE_NRF528XX )

#elif ( GSM_USE_SAM_DUE )
  #define Uart      UARTClass
#elif ( GSM_USING_SOFTWARE_SERIAL )
  #define Uart      SoftwareSerial
#else
  #define Uart      HardwareSerial
#endif

#if !defined(UBLOX_USING_RESET_PIN)
  #define UBLOX_USING_RESET_PIN    false
  #warning No UBLOX_USING_RESET_PIN
#elif UBLOX_USING_RESET_PIN
  #warning UBLOX_USING_RESET_PIN  
#endif

#if !defined(UBLOX_USING_LOW_POWER_MODE)
  #define UBLOX_USING_LOW_POWER_MODE    false
  #warning No UBLOX_USING_LOW_POWER_MODE
#elif UBLOX_USING_LOW_POWER_MODE
  #warning UBLOX_USING_LOW_POWER_MODE   
#endif

class ModemUrcHandler 
{
  public:
    virtual void handleUrc(const String& urc) = 0;
};


//////////////////////////////////////////////////////

#define MAX_URC_HANDLERS                          10 // 7 sockets + GPRS + GSMLocation + GSMVoiceCall
#define MODEM_MIN_RESPONSE_OR_URC_WAIT_TIME_MS    20

class ModemClass 
{
  /////////////////////////////////////////

  private:

#if ( GSM_USE_SAMD || GSM_USE_NRF528XX )
    Uart* _uart;
#elif ( GSM_USING_SOFTWARE_SERIAL )
    SoftwareSerial*   _uart;
#else
    HardwareSerial*   _uart;
#endif
    
    unsigned long _baud;
    
    int _resetPin;     
    int _dtrPin;
    bool _lowPowerMode;

    unsigned long _lastResponseOrUrcMillis;

    enum 
    {
      AT_COMMAND_IDLE,
      AT_RECEIVING_RESPONSE
    } _atCommandState;
    
    int _ready;
    String _buffer;
    String* _responseDataStorage;

    static ModemUrcHandler* _urcHandlers[MAX_URC_HANDLERS];
    static Print* _debugPrint;
    
  public:
  

    ModemClass(Uart& uart, unsigned long baud, int resetPin = -1, int dtrPin = -1) :
      _uart(&uart),
      _baud(baud > 115200 ? 115200 : baud),   
      _resetPin(resetPin),    
      _dtrPin(dtrPin),
      _lowPowerMode(false),
      _lastResponseOrUrcMillis(0),
      _atCommandState(AT_COMMAND_IDLE),
      _ready(GSM_RESPONSE_OK /*1*/),
      _responseDataStorage(NULL)
    {
      _buffer.reserve(64);
    }
    
    /////////////////////////////////////////
    
    int begin(unsigned long baud, bool restart = true)
    {
      (void) restart;

     
#if UBLOX_USING_SET_BAUD
      bool newBaud = false;
      
      if (_baud != baud)
      {
        _baud = ( baud > 115200 ? 115200 : baud );
        newBaud = true;
      }
#else    
         
      if (_baud != baud)
      {
        _baud = ( baud > 115200 ? 115200 : baud );
      }
#endif
        
      GSM_LOGDEBUG1(F("begin: UART baud = "), _baud);  


#if !defined(ARDUINO_SAMD_MKRGSM1400)
      #warning Using SerialGSM.begin
      #if (ESP32)
        SerialGSM.updateBaudRate(_baud > 115200 ? 115200 : _baud);
        SerialGSM.begin(_baud > 115200 ? 115200 : _baud);
      #else  
        SerialGSM.begin(_baud > 115200 ? 115200 : _baud);
      #endif
#else 
      _uart->begin(_baud > 115200 ? 115200 : _baud);
#endif
      
#if UBLOX_USING_RESET_PIN
      if (_resetPin > -1 && restart) 
      {
        GSM_LOGDEBUG(F("begin: Reset"));
        pinMode(_resetPin, OUTPUT);
        digitalWrite(_resetPin, HIGH);
        delayMicroseconds(150);
        digitalWrite(_resetPin, LOW);
        delay(3);
        digitalWrite(_resetPin, HIGH);
        delay(600);
        digitalWrite(_resetPin, LOW);
        GSM_LOGDEBUG(F("begin: Reset done"));
      } 
      else  
      {
        if (!autosense()) 
        {
          GSM_LOGDEBUG(F("begin: nonrestart autosense error"));
          return GSM_MODEM_START_ERROR;
        }

        if (!reset()) 
        {
          GSM_LOGDEBUG(F("begin: reset error"));
          return GSM_MODEM_START_ERROR;
        }
      }
#else
      if (!reset()) 
      {
        GSM_LOGDEBUG(F("begin: reset error"));
        return GSM_MODEM_START_ERROR;
      }
#endif
      
      GSM_LOGDEBUG(F("begin: Check autosense"));

      if (!autosense()) 
      {
        GSM_LOGDEBUG(F("begin: autosense error"));
        return GSM_MODEM_START_ERROR;
      }
      
#if UBLOX_USING_SET_BAUD
  
      GSM_LOGDEBUG(F("begin: Check baud"));

      // KH, must always set baud here
      // u-blox SARA and LISA can auto=adjust baurate, and we don't need to set baud
      // unless UBLOX_USING_SET_BAUD is turned ON if modem don't have this auto feature
      if ( restart || newBaud )
      {
        GSM_LOGDEBUG1(F("begin: Set baud = "), _baud);
        
        sendf("AT+IPR=%ld", _baud);
        
        if (waitForResponse() != GSM_RESPONSE_OK) 
        {
          GSM_LOGDEBUG(F("begin: Set baud error"));
          return GSM_MODEM_START_ERROR;
        }

        _uart->end();
        delay(100);
        _uart->begin(_baud);

        if (!autosense()) 
        {
          GSM_LOGDEBUG(F("begin: Set baud autosense error"));
          return GSM_MODEM_START_ERROR;
        }
      }
      
#endif

#if UBLOX_USING_LOW_POWER_MODE 
      if (_dtrPin > -1) 
      {
        pinMode(_dtrPin, OUTPUT);
        noLowPowerMode();

        send("AT+UPSV=3");
        
        if (waitForResponse() != GSM_RESPONSE_OK) 
        {
          GSM_LOGDEBUG(F("begin: DTR response error"));
          return GSM_MODEM_START_ERROR;
        }
      }
#endif
           
      GSM_LOGDEBUG(F("begin: Modem OK"));

      return GSM_MODEM_START_OK;
    }
    
    /////////////////////////////////////////
    
    int begin(bool restart = true)
    {
      return begin(_baud, restart);
    }
    
    /////////////////////////////////////////
       
    void end()
    {
      _uart->end();
      
#if UBLOX_USING_RESET_PIN      
      digitalWrite(_resetPin, HIGH);
#endif

#if UBLOX_USING_LOW_POWER_MODE 
      if (_dtrPin > -1) 
      {
        digitalWrite(_dtrPin, LOW);
      }
#endif
      
    }
    
    /////////////////////////////////////////

    void debug()
    {
      debug(Serial);
    }
    
    /////////////////////////////////////////

    void debug(Print& p)
    {
      _debugPrint = &p;
    }

    /////////////////////////////////////////

    void noDebug()
    {
      _debugPrint = NULL;
    }
    
    /////////////////////////////////////////

    int autosense(unsigned int timeout = 10000)
    {
      for (unsigned long start = millis(); (millis() - start) < timeout;) 
      {
        if (noop() == 1) 
        {
          return 1;
        }

        delay(100);
      }

      return 0;
    }
    
    /////////////////////////////////////////

    int noop()
    {
      send("AT");

      return (waitForResponse() == GSM_RESPONSE_OK);
    }

    /////////////////////////////////////////
    
    bool reset()
    {
      //Flush Buffer 
      waitForResponse(1);
      
      //Reseting  
      send("AT+CFUN=16");
      
      //Not got OK, error
      if(!(waitForResponse(5000) ==  GSM_RESPONSE_OK))
        return(false);
        
      //Time to u-blox (lisa u200) send sometimes asyncronous data, without OK,13,10
      send("AT");
      delay(3000);	
      
      ////Flush Buffer 
      waitForResponse(1);
      
      //Still Alive
      send("AT");
      return (waitForResponse(3000) == GSM_RESPONSE_OK);
    }
    
    /////////////////////////////////////////

    String getModemName() 
    {    
#if ( GSM_MODEM_UBLOX || TINY_GSM_MODEM_UBLOX )
      String name = "u-blox GSM_GPRS";
#elif ( GSM_MODEM_SARAR4 || TINY_GSM_MODEM_SARAR4 )
      String name = "u-blox SARA_R4";
#else
      String name = "";      
#endif

      send("AT+GMM");
      
      String modemName;
      modemName.reserve(48);
      
      if ( waitForResponse(100, &modemName) != GSM_RESPONSE_OK) 
      { 
        //GSM_LOGDEBUG1("### Modem Name :", name);
        return name; 
      }
      
      //GSM_LOGDEBUG1("### Modem Name :", modemName);

      return modemName;
    }
    
    /////////////////////////////////////////
    
    String getModemInfo() 
    {
      send("ATI");
      
      String modemInfo;
      modemInfo.reserve(48);
      
      waitForResponse(100, &modemInfo);
      
      //GSM_LOGDEBUG1("### Modem Info :", modemInfo);

      return modemInfo;
    }
   
    /////////////////////////////////////////

    int lowPowerMode()
    {
#if UBLOX_USING_LOW_POWER_MODE    
      if (_dtrPin > -1) 
      {
        _lowPowerMode = true;

        digitalWrite(_dtrPin, HIGH);
        
        GSM_LOGDEBUG(F("lowPowerMode: DTR HIGH, enter lowPowerMode"));

        return 1;
      }

      return 0;
#else
      return 1;
#endif      
    }
   
    /////////////////////////////////////////

    int noLowPowerMode()
    {
#if UBLOX_USING_LOW_POWER_MODE    
      if (_dtrPin > -1) 
      {
        _lowPowerMode = false;

        digitalWrite(_dtrPin, LOW);
        
        GSM_LOGDEBUG(F("lowPowerMode: DTR LOW, exit lowPowerMode"));

        return 1;
      }

      return 0;
#else
      return 1;
#endif      
    }
     
    /////////////////////////////////////////

    void closeSocket(int socket)
    {
      sendf("AT+USOCL=%d", socket);
    }
    
    /////////////////////////////////////////

    size_t write(uint8_t c)
    {
      return _uart->write(c);
    }
    
    /////////////////////////////////////////

    size_t write(const uint8_t* buf, size_t size)
    {
      return _uart->write(buf, size);
    }
    
    /////////////////////////////////////////

    void send(const char* command)
    {
#if UBLOX_USING_LOW_POWER_MODE    
      if (_lowPowerMode) 
      {
        digitalWrite(_dtrPin, LOW);
        delay(5);
      }
#endif

      if (_debugPrint) 
      {
        _debugPrint->write(command);
        _debugPrint->write(GSM_NL);
      }

      // compare the time of the last response or URC and ensure
      // at least 20ms have passed before sending a new command
      unsigned long delta = millis() - _lastResponseOrUrcMillis;
      
      if (delta < MODEM_MIN_RESPONSE_OR_URC_WAIT_TIME_MS) 
      {
        delay(MODEM_MIN_RESPONSE_OR_URC_WAIT_TIME_MS - delta);
      }

      _uart->println(command);
      _uart->flush();
      _atCommandState = AT_COMMAND_IDLE;
      _ready = GSM_RESPONSE_IDLE;   //0;
    }
   
    /////////////////////////////////////////
    
    void send(const String& command) 
    {
      send(command.c_str());
    }
    
    /////////////////////////////////////////
    
    void sendf(const char *fmt, ...)
    {
      char buf[BUFSIZ];

      va_list ap;
      va_start((ap), (fmt));
      vsnprintf(buf, sizeof(buf) - 1, fmt, ap);
      va_end(ap);

      send(buf);
    }
        
    /////////////////////////////////////////

    int waitForResponse(unsigned long timeout = 100, String* responseDataStorage = NULL)
    {
      _responseDataStorage = responseDataStorage;
      
      for (unsigned long start = millis(); (millis() - start) < timeout;) 
      {
        int r = ready();

        if (r != GSM_RESPONSE_IDLE) 
        {
          _responseDataStorage = NULL;
          
          return r;
        }
      }

      _responseDataStorage = NULL;
      _buffer = "";
      
      return GSM_RESPONSE_TIMEOUT;    //-1;
    }
    
    /////////////////////////////////////////

    int waitForPrompt(unsigned long timeout = 500)
    {
      for (unsigned long start = millis(); (millis() - start) < timeout;) 
      {
        ready();

        if (_buffer.endsWith(">")) 
        {
          return 1;
        }
      }

      return -1;
    }   
    
    /////////////////////////////////////////

    int ready()
    {
      poll();

      return _ready;
    }
    
    /////////////////////////////////////////

    void poll()
    {
      while (_uart->available()) 
      {
        char c = _uart->read();

        if (_debugPrint) 
        {
          _debugPrint->write(c);
        }

        _buffer += c;

        switch (_atCommandState) 
        {
          case AT_COMMAND_IDLE:
          default: 
          {

              if (_buffer.startsWith("AT") && _buffer.endsWith("\r\n")) 
              {
                _atCommandState = AT_RECEIVING_RESPONSE;
                _buffer = "";
              }  
              else if (_buffer.endsWith("\r\n")) 
              {
                _buffer.trim();

                if (_buffer.length()) 
                {
                  _lastResponseOrUrcMillis = millis();

                  for (int i = 0; i < MAX_URC_HANDLERS; i++) 
                  {
                    if (_urcHandlers[i] != NULL) 
                    {
                      _urcHandlers[i]->handleUrc(_buffer);
                    }
                  }
                }

                _buffer = "";
              }

              break;
            }

          case AT_RECEIVING_RESPONSE: 
          {
              if (c == '\n') 
              {
                _lastResponseOrUrcMillis = millis();

                int responseResultIndex = _buffer.lastIndexOf("OK\r\n");
                
                if (responseResultIndex != -1) 
                {
                  _ready = GSM_RESPONSE_OK;               //1;
                } 
                else 
                {
                  responseResultIndex = _buffer.lastIndexOf("ERROR\r\n");
                  
                  if (responseResultIndex != -1) 
                  {
                    _ready = GSM_RESPONSE_ERROR;          //2;
                  } 
                  else 
                  {
                    responseResultIndex = _buffer.lastIndexOf("NO CARRIER\r\n");
                    
                    if (responseResultIndex != -1) 
                    {
                      _ready = GSM_RESPONSE_NO_CARRIER;   //3;
                    }
                  }
                }

                if (_ready != GSM_RESPONSE_IDLE /*0*/) 
                {
#if UBLOX_USING_LOW_POWER_MODE                
                  if (_lowPowerMode) 
                  {
                    digitalWrite(_dtrPin, HIGH);
                  }
#endif

                  if (_responseDataStorage != NULL) 
                  {
                    _buffer.remove(responseResultIndex);
                    _buffer.trim();

                    *_responseDataStorage = _buffer;

                    _responseDataStorage = NULL;
                  }

                  _atCommandState = AT_COMMAND_IDLE;
                  _buffer = "";
                  
                  return;
                }
              }
              
              break;
            }
        }
      }
    }

    /////////////////////////////////////////

    void setResponseDataStorage(String* responseDataStorage)
    {
      _responseDataStorage = responseDataStorage;
    }
    
    /////////////////////////////////////////

    void addUrcHandler(ModemUrcHandler* handler)
    {
      for (int i = 0; i < MAX_URC_HANDLERS; i++) 
      {
        if (_urcHandlers[i] == NULL) 
        {
          _urcHandlers[i] = handler;
          break;
        }
      }
    }
    
    /////////////////////////////////////////

    void removeUrcHandler(ModemUrcHandler* handler)
    {
      for (int i = 0; i < MAX_URC_HANDLERS; i++) 
      {
        if (_urcHandlers[i] == handler) 
        {
          _urcHandlers[i] = NULL;
          break;
        }
      }
    }
    
    /////////////////////////////////////////

    void setBaudRate(unsigned long baud)
    {
      _baud = baud;
    }
    
    
    /////////////////////////////////////////

    bool setRAT(const char* act) 
    {
      sendf("AT+URAT=%s", act);
      
      if (waitForResponse(10000) == GSM_RESPONSE_OK) 
      {
        GSM_LOGDEBUG(F("GSMBand::setRAT: OK"));
        
        return true;
      }
      
      GSM_LOGDEBUG(F("GSMBand::setRAT: Failed"));
      
      return false;
    }
    
    /////////////////////////////////////////

    String getBand()
    {
      String response;

      send("AT+UBANDSEL?");

      if (waitForResponse(100, &response) == GSM_RESPONSE_OK) 
      {
        GSM_LOGDEBUG1(F("GSMBand::getBand: response = "), response);
        
        if (response.startsWith("+UBANDSEL: ")) 
        {
          response.remove(0, 11);

          if (response == "900") 
          {
            GSM_LOGDEBUG(F("GSMBand::getBand: GSM_MODE_EGSM / 900"));
            
            return GSM_MODE_EGSM;
          } 
          else if (response == "1800") 
          {
            GSM_LOGDEBUG(F("GSMBand::getBand: GSM_MODE_DCS / 1800"));
            
            return GSM_MODE_DCS;
          } 
          else if (response == "1900") 
          {
            GSM_LOGDEBUG(F("GSMBand::getBand: GSM_MODE_PCS / 1900"));
            
            return GSM_MODE_PCS;
          } 
          else if (response == "900,1800") 
          {
            GSM_LOGDEBUG(F("GSMBand::getBand: GSM_MODE_EGSM_DCS / 900 + 1800"));
            
            return GSM_MODE_EGSM_DCS;
          } 
          else if (response == "850,1900") 
          {
            GSM_LOGDEBUG(F("GSMBand::getBand: GSM_MODE_GSM850_PCS / 850 + 1900"));
            
            return GSM_MODE_GSM850_PCS;
          } 
          else if (response == "850,900,1800,1900") 
          {
            GSM_LOGDEBUG(F("GSMBand::getBand: GSM_MODE_GSM850_EGSM_DCS_PCS / 850 + 900 + 1800 + 1900"));
            
            return GSM_MODE_GSM850_EGSM_DCS_PCS;
          } 
          else if (response == "2100") 
          {
            GSM_LOGDEBUG(F("GSMBand::getBand: GSM_MODE_UMTS / 2100"));
            
            return GSM_MODE_UMTS;
          } 
          else if (response == "850,900,1900,2100") 
          {
            GSM_LOGDEBUG(F("GSMBand::getBand: GSM_MODE_GSM850_EGSM_PCS_UMTS / 850 + 900 + 1900 + 2100"));
            
            return GSM_MODE_GSM850_EGSM_PCS_UMTS;
          }
        }
      }

      return GSM_MODE_UNDEFINED;
    }
    
    /////////////////////////////////////////

    bool setBand(String band)
    {
      const char* bands;
      
      GSM_LOGDEBUG1(F("GSMBand::setBand: input band = "), band);

      // Set the Radio Access Technology to support the 1800 MHz frequency
      // in accord with the bands selected
      if (band == "DCS_MODE" || band == "EGSM_DCS_MODE" || band == "GSM850_EGSM_DCS_PCS_MODE") 
      {    
        setRAT(GSM_BANDS);
      } 
      else 
      {
        setRAT(UMTS_BANDS);
      }

      if (band == GSM_MODE_EGSM) 
      {
        bands = "900";
      } 
      else if (band == GSM_MODE_DCS) 
      {
        bands = "1800";
      } 
      else if (band == GSM_MODE_PCS) 
      {
        bands = "1900";
      } 
      else if (band == GSM_MODE_EGSM_DCS) 
      {
        bands = "900,1800";
      } 
      else if (band == GSM_MODE_GSM850_PCS) 
      {
        bands = "850,1900";
      } 
      else if (band == GSM_MODE_GSM850_EGSM_DCS_PCS) 
      {
        bands = "850,900,1800,1900";
      } 
      else if (band == GSM_MODE_UMTS) 
      {
        bands = "2100";
      } 
      else if (band == GSM_MODE_GSM850_EGSM_PCS_UMTS) 
      {
        bands = "850,900,1900,2100";
      } 
      else 
      {
        return false;
      }
      
      GSM_LOGDEBUG1(F("GSMBand::setBand: bands = "), bands);

      for (int i = 0; i < 10; i++) 
      {
        sendf("AT+UBANDSEL=%s", bands);
        int result = waitForResponse(10000);

        if (result == GSM_RESPONSE_OK) 
        {
          GSM_LOGDEBUG(F("GSMBand::setBand: OK"));
          
          return true;
        } 
        else if (result == GSM_RESPONSE_ERROR) 
        {
          GSM_LOGDEBUG(F("GSMBand::setBand: failed"));
          
          return false;
        }

        // retry ...
        delay(100);
      }
      
      GSM_LOGDEBUG(F("GSMBand::setBand: failed after retries"));

      return false;
    }
    
    /////////////////////////////////////////
    //From GSMFileUtils_Generic
    
    void setReportError()
    {
      send("AT+CMEE=2");
    }
    
    /////////////////////////////////////////
    
    void listFile()
    {
      send("AT+ULSTFILE=0");
    }
    
    void fileSize(const String filename)
    {
      sendf("AT+ULSTFILE=2,\"%s\"", filename.c_str());
    }
    
    void freeSpace()
    {
      send("AT+ULSTFILE=1");
    }
    
    void deleteFile(const String filename)
    {
      // Delete file +UDELFILE
      sendf("AT+UDELFILE=\"%s\"", filename.c_str());
    }
    
    void readBlock(const String filename, const uint32_t offset, const uint32_t len)
    {
      sendf("AT+URDBLOCK=\"%s\",%d,%d", filename.c_str(), offset * 2, len * 2);
    }
    
    /////////////////////////////////////////
    
    void downloadFile(const String filename, uint32_t size)
    {
      sendf("AT+UDWNFILE=\"%s\",%d", filename.c_str(), size * 2);
    }
    
    /////////////////////////////////////////
    
    void readFile(const String filename)
    {
      sendf("AT+URDFILE=\"%s\"", filename.c_str());
    }
    
    /////////////////////////////////////////
    //From GSMHttpUtils_Generic
    
    void setSignedCertificate(const char* name, size_t size)
    {
      sendf("AT+USECMNG=0,1,\"%s\",%d", name, size);
    }
    
    void setPrivateKey(const char* name, size_t size) 
    {
      // SSL/TLS certificates and private keys manager +USECMNG
      sendf("AT+USECMNG=0,2,\"%s\",%d", name, size);
    }
    
    void setTrustedRoot(const char* name) 
    {
      //SSL/TLS/DTLS security layer profile manager +USECPRF
      sendf("AT+USECPRF=0,3,\"%s\"", name);
    }
    
    void useSignedCertificate(const char* name) 
    {
      //SSL/TLS/DTLS security layer profile manager +USECPRF
      sendf("AT+USECPRF=0,5,\"%s\"", name);
    }  
    
    void usePrivateKey(const char* name) 
    {
      //SSL/TLS/DTLS security layer profile manager +USECPRF
      sendf("AT+USECPRF=0,6,\"%s\"", name);
    }
    
    void eraseAllCertificates(int cert_type) 
    {       
      // SSL/TLS certificates and private keys manager +USECMNG
      sendf("AT+USECMNG=3,%d", cert_type);
    }
    
    void eraseCert(const char* name, int type) 
    {
      // SSL/TLS certificates and private keys manager +USECMNG
      sendf("AT+USECMNG=2,%d,\"%s\"", type, name);
    }
    
    bool handleUrcFileUtils(const String& urc)
    {
      if (urc.startsWith("+UUHTTPCR: ")) 
      {
        //_httpresp = false;
        return false;
        
        if (urc.endsWith(",1")) 
        {
          //_httpresp = true;
          return true;
        }
      }
      
      return false;
    }
    
    void enableSSL() 
    {
      // Sets the profile ID #1
      //SSL/TLS/DTLS security layer profile manager +USECPRF
      sendf("AT+USECPRF=0,0,1");
    }
    
    void disableSSL() 
    {
      // Sets the profile ID #1
      //SSL/TLS/DTLS security layer profile manager +USECPRF
      sendf("AT+USECPRF=0,0,0");
    }
    
    void configServerProfile0()
    {
      send("AT+UHTTP=0");
    }
    
    void configServerNameProfile0(const char* url)
    {
      sendf("AT+UHTTP=0,1,\"%s\"", url);
    }
    
    void configServerPortProfile0(int httpport)
    {
      sendf("AT+UHTTP=0,5,%d", httpport);
    }
    
    void configServerSSLProfile0()
    {
      send("AT+UHTTP=0,6,1,0");
    }
    
    void resolveDNS(const char* url)
    {
      sendf("AT+UDNSRN=0,\"%s\"", url);
    }
    
    void headRequest(const char* path, const char* filename) 
    {
      // Makes a HEAD request and store the response in _file
      // HTTP command +UHTTPC
      sendf("AT+UHTTPC=0,0,\"%s\",\"%s\"", path, filename);
    }
    
    void getRequest(const char* path, const char* filename) 
    {
      // Makes a GET request and store it in _file
      // HTTP command +UHTTPC
      sendf("AT+UHTTPC=0,1,\"%s\",\"%s\"", path, filename);
    }
    
    void deleteRequest(const char* path, const char* filename) 
    {
      // make a DELETE request and store it in _file
      // HTTP command +UHTTPC
      sendf("AT+UHTTPC=0,2,\"%s\",\"%s\"", path, filename);
    }
    
    void putRequest(const char* path, const char* filename) 
    {
      // make a PUT request and store it in _file
      sendf("AT+UHTTPC=0,3,\"%s\",\"%s\"", path, filename);
    }
    
    void postRequest(const char* path, const char* filename) 
    {
      // make a POST request and store it in _file
      // HTTP command +UHTTPC
      sendf("AT+UHTTPC=0,4,\"%s\",\"%s\"", path, filename);
    }
    
    /////////////////////////////////////////
    //From GSMLocation_Generic_Impl.hpp
    
    void deepScanLocation()
    {
      send("AT+ULOCCELL=1");
    }
    
    void getLocation()
    {
      // Ask for localization information +ULOC. Single Shot, Using CellLocate, Std Response, timeout 1s, accuracy 1 meter
      send("AT+ULOC=2,2,0,1,1");
    }
    
    /////////////////////////////////////////
    //From GSM_Generic_Impl.hpp
    
    int isAccessAlive()
    {
      String response;

      send("AT+CREG?");
      
      if (waitForResponse(100, &response) == GSM_RESPONSE_OK) 
      {
        int status = response.charAt(response.length() - 1) - '0';

        if (status == GSM_NETWORK_HOME || status == GSM_NETWORK_ROAMING || status == GSM_NETWORK_EMERGENCY_ONLY) 
        {
          GSM_LOGDEBUG(F("GSM::isAccessAlive: alive"));
          return 1;
        }
      }

      GSM_LOGDEBUG(F("GSM::isAccessAlive: not alive"));
      
      return 0;
    }
    
    bool shutdown(GSM3_NetworkStatus_t& _state)
    {
      if (_state == GSM_READY) 
      {
        send("AT+CPWROFF");
        waitForResponse(40000);
      }
      
      end();
      _state = GSM_OFF;
      
      return true;
    }
    
    int GSM_ready(GSM_Data& _gsmData, String& _response)
    {
      if (_gsmData._state == GSM_ERROR) 
      {
        GSM_LOGDEBUG(F("GSM::ready: _gsmData._state = GSM_ERROR"));
        
        return GSM_RESPONSE_ERROR;
      }

      int modemReady = ready();

      if (modemReady == GSM_RESPONSE_IDLE) 
      {
        return GSM_RESPONSE_IDLE;
      }
      
      switch (_gsmData._readyState) 
      {
        case READY_STATE_CHECK_SIM: 
          {
            setResponseDataStorage(&_response);
            send("AT+CPIN?");
            _gsmData._readyState = READY_STATE_WAIT_CHECK_SIM_RESPONSE;
            modemReady = GSM_RESPONSE_IDLE;
            
            break;
          }

        case READY_STATE_WAIT_CHECK_SIM_RESPONSE: 
          {
            if (modemReady > GSM_RESPONSE_OK) 
            {
              // error => retry
              _gsmData._readyState = READY_STATE_CHECK_SIM;
              modemReady = GSM_RESPONSE_IDLE;
            } 
            else 
            {
              if (_response.endsWith("READY")) 
              {
                _gsmData._readyState = READY_STATE_SET_PREFERRED_MESSAGE_FORMAT;
                modemReady = GSM_RESPONSE_IDLE;
              } 
              else if (_response.endsWith("SIM PIN")) 
              {
                _gsmData._readyState = READY_STATE_UNLOCK_SIM;
                modemReady = GSM_RESPONSE_IDLE;
              } 
              else 
              {
                _gsmData._state = GSM_ERROR;
                modemReady = GSM_RESPONSE_ERROR;
              }
            }

            break;
          }

        case READY_STATE_UNLOCK_SIM: 
          {
            if (_gsmData._pin != NULL) 
            {
              setResponseDataStorage(&_response);
              sendf("AT+CPIN=\"%s\"", _gsmData._pin);

              _gsmData._readyState = READY_STATE_WAIT_UNLOCK_SIM_RESPONSE;
              modemReady = GSM_RESPONSE_IDLE;
            } 
            else 
            {
              _gsmData._state = GSM_ERROR;
              modemReady = GSM_RESPONSE_ERROR;
            }
            
            break;
          }

        case READY_STATE_WAIT_UNLOCK_SIM_RESPONSE: 
          {
            if (modemReady > GSM_RESPONSE_OK) 
            {
              _gsmData._state = GSM_ERROR;
              modemReady = GSM_RESPONSE_ERROR;
            } 
            else 
            {
              _gsmData._readyState = READY_STATE_SET_PREFERRED_MESSAGE_FORMAT;
              modemReady = GSM_RESPONSE_IDLE;
            }

            break;
          }

        case READY_STATE_SET_PREFERRED_MESSAGE_FORMAT: 
          {
            send("AT+CMGF=1");
            _gsmData._readyState = READY_STATE_WAIT_SET_PREFERRED_MESSAGE_FORMAT_RESPONSE;
            modemReady = GSM_RESPONSE_IDLE;
            
            break;
          }

        case READY_STATE_WAIT_SET_PREFERRED_MESSAGE_FORMAT_RESPONSE: 
          {
            if (modemReady > GSM_RESPONSE_OK) 
            {
              _gsmData._state = GSM_ERROR;
              modemReady = GSM_RESPONSE_ERROR;
            } 
            else 
            {
              _gsmData._readyState = READY_STATE_SET_HEX_MODE;
              modemReady = GSM_RESPONSE_IDLE;
            }

            break;
          }

        case READY_STATE_SET_HEX_MODE: 
          {
            send("AT+UDCONF=1,1");
            _gsmData._readyState = READY_STATE_WAIT_SET_HEX_MODE;
            modemReady = GSM_RESPONSE_IDLE;
            
            break;
          }

        case READY_STATE_WAIT_SET_HEX_MODE: 
          {
            if (modemReady > GSM_RESPONSE_OK) 
            {
              _gsmData._state = GSM_ERROR;
              modemReady = GSM_RESPONSE_ERROR;
            } 
            else 
            {
              _gsmData._readyState = READY_STATE_SET_AUTOMATIC_TIME_ZONE;
              modemReady = GSM_RESPONSE_IDLE;
            }

            break;
          }

        case READY_STATE_SET_AUTOMATIC_TIME_ZONE: 
          {
            send("AT+CTZU=1");
            _gsmData._readyState = READY_STATE_WAIT_SET_AUTOMATIC_TIME_ZONE_RESPONSE;
            modemReady = GSM_RESPONSE_IDLE;
            
            break;
          }

        case READY_STATE_WAIT_SET_AUTOMATIC_TIME_ZONE_RESPONSE: 
          {
            if (modemReady > GSM_RESPONSE_OK) 
            {
              _gsmData._state = GSM_ERROR;
              modemReady = GSM_RESPONSE_ERROR;
            } 
            else 
            {
              _gsmData._readyState = READY_STATE_ENABLE_DTMF_DETECTION;
              modemReady = GSM_RESPONSE_IDLE;
            }

            break;
          }

        case READY_STATE_ENABLE_DTMF_DETECTION: 
          {
            send("AT+UDTMFD=1,2");
            _gsmData._readyState = READY_STATE_WAIT_ENABLE_DTMF_DETECTION_RESPONSE;
            modemReady = GSM_RESPONSE_IDLE;
            
            break;
          }

        case READY_STATE_WAIT_ENABLE_DTMF_DETECTION_RESPONSE: 
          {
            if (modemReady > GSM_RESPONSE_OK) 
            {
              _gsmData._state = GSM_ERROR;
              modemReady = GSM_RESPONSE_ERROR;
            } 
            else 
            {
              _gsmData._readyState = READY_STATE_CHECK_REGISTRATION;
              modemReady = GSM_RESPONSE_IDLE;
            }

            break;
          }

        case READY_STATE_CHECK_REGISTRATION: 
          {
            setResponseDataStorage(&_response);
            send("AT+CREG?");
            _gsmData._readyState = READY_STATE_WAIT_CHECK_REGISTRATION_RESPONSE;
            modemReady = GSM_RESPONSE_IDLE;
            
            break;
          }

        case READY_STATE_WAIT_CHECK_REGISTRATION_RESPONSE: 
          {
            if (modemReady > GSM_RESPONSE_OK) 
            {
              _gsmData._state = GSM_ERROR;
              modemReady = GSM_RESPONSE_ERROR;
            } 
            else 
            {
              int status = _response.charAt(_response.length() - 1) - '0';

              if (status == GSM_NETWORK_NOT_REGISTERED_NOT_SEARCHING || status == GSM_NETWORK_UNKNOWN) 
              {
                _gsmData._readyState = READY_STATE_CHECK_REGISTRATION;
                modemReady = GSM_RESPONSE_IDLE;
              } 
              else if (status == GSM_NETWORK_HOME || status == GSM_NETWORK_ROAMING || status == GSM_NETWORK_EMERGENCY_ONLY) 
              {
                _gsmData._readyState = READY_STATE_SET_REPORTING_CALL_STATUS;
                _gsmData._state = GSM_READY;
                modemReady = GSM_RESPONSE_IDLE;
              } 
              else if (status == GSM_NETWORK_NOT_REGISTERED_SEARCHING) 
              {
                _gsmData._readyState = READY_STATE_CHECK_REGISTRATION;
                _gsmData._state = GSM_CONNECTING;
                modemReady = GSM_RESPONSE_IDLE;
              } 
              else if (status == GSM_NETWORK_REGISTRATION_DENIED) 
              {
                _gsmData._state = GSM_ERROR;
                modemReady = GSM_RESPONSE_ERROR;
              }
            }

            break;
          }

        case READY_STATE_SET_REPORTING_CALL_STATUS: 
          {
            send("AT+UCALLSTAT=1");
            _gsmData._readyState = READY_STATE_WAIT_SET_REPORTING_CALL_STATUS;
            modemReady = GSM_RESPONSE_IDLE;
            
            break;
          }

        case READY_STATE_WAIT_SET_REPORTING_CALL_STATUS: 
          {
            if (modemReady > GSM_RESPONSE_OK) 
            {
              _gsmData._state = GSM_ERROR;
              modemReady = GSM_RESPONSE_ERROR;
            } 
            else 
            {
              _gsmData._readyState = READY_STATE_DONE;
              modemReady = GSM_RESPONSE_OK;
            }

            break;
          }

        case READY_STATE_DONE:
          break;
      }

      return modemReady;
    }
    
    unsigned long getTime()
    {
      String response;

      send("AT+CCLK?");
      
      if (waitForResponse(100, &response) != GSM_RESPONSE_OK) 
      {
        return 0;
      }

      struct tm now;

      if (strptime(response.c_str(), "+CCLK: \"%y/%m/%d,%H:%M:%S", &now) != NULL)
      {
        // adjust for timezone offset which is +/- in 15 minute increments
        GSM_LOGDEBUG1(F("GSM::getTime: response = "), response);

        time_t result = mktime(&now);
        time_t delta = ((response.charAt(26) - '0') * 10 + (response.charAt(27) - '0')) * (15 * 60);

        if (response.charAt(25) == '-') 
        {
          result += delta;
        } 
        else if (response.charAt(25) == '+') 
        {
          result -= delta;
        }

        return result;
      }

      return 0;
    }
    
    unsigned long getLocalTime()
    {
      String response;

      send("AT+CCLK?");
      
      if (waitForResponse(100, &response) != GSM_RESPONSE_OK) 
      {
        return 0;
      }

      struct tm now;

      if (strptime(response.c_str(), "+CCLK: \"%y/%m/%d,%H:%M:%S", &now) != NULL) 
      {
        GSM_LOGDEBUG1(F("GSM::getLocalTime: response = "), response);
        
        time_t result = mktime(&now);
        return result;
      }

      return 0;
    }
    
    /////////////////////////////////////////
    //From GSMPIN_Generic_Impl.hpp

    void readPIN()
    {
      send("AT+CPIN?");
    }
    
    void checkPIN(String pin)
    {
      sendf("AT+CPIN=\"%s\"", pin.c_str());
    }
    
    void checkPUK(String puk, String pin)
    {
      sendf("AT+CPIN=\"%s\",\"%s\"", puk.c_str(), pin.c_str());
    }
    
    void changePIN(String old, String pin)
    {
      sendf("AT+CPWD=\"SC\",\"%s\",\"%s\"", old.c_str(), pin.c_str());
    }
    
    void switchPIN(String pin, bool& _pinUsed)
    {
      String response;
      
      GSM_LOGDEBUG1(F("GSMPIN::switchPIN: pin = "), pin);

      // Facility lock +CLCK => query SIM status
      send("AT+CLCK=\"SC\",2");
      
      if (waitForResponse(180000, &response) != GSM_RESPONSE_OK) 
      {
        GSM_LOGDEBUG(F("GSMPIN::switchPIN: Error no response"));
        return;
      }

      if (response == "+CLCK: 0") 
      {
        // Facility lock +CLCK => Lock SIM
        sendf("AT+CLCK=\"SC\",1,\"%s\"", pin.c_str());
        
        if (waitForResponse(180000, &response) == GSM_RESPONSE_OK) 
        {
          GSM_LOGDEBUG(F("GSMPIN::switchPIN: OK. PIN lock on."));
          _pinUsed = true;
        } 
        else 
        {
          GSM_LOGDEBUG(F("GSMPIN::switchPIN: Error +CLCK: 0"));
          _pinUsed = false;
        }
      } 
      else if (response == "+CLCK: 1") 
      {
        // Facility lock +CLCK => Unlock SIM
        sendf("AT+CLCK=\"SC\",0,\"%s\"", pin.c_str());
        
        if (waitForResponse(180000, &response) == GSM_RESPONSE_OK) 
        {
          GSM_LOGDEBUG(F("GSMPIN::switchPIN: OK. PIN lock off."));
          _pinUsed = false;
        } 
        else 
        {
          GSM_LOGDEBUG(F("GSMPIN::switchPIN: Error +CLCK: 1"));
          _pinUsed = true;
        }
      } 
      else 
      {
        GSM_LOGDEBUG(F("GSMPIN::switchPIN: Error"));
      }
    }
    
    int checkReg()
    {
      for (unsigned long start = millis(); (millis() - start) < 10000L;) 
      {
        // Network registration status +CREG
        send("AT+CREG?");

        String response = "";

        if (waitForResponse(100, &response) == GSM_RESPONSE_OK) 
        {
          if (response.startsWith("+CREG: ")) 
          {
            if (response.endsWith(",1")) 
            {
              return GSM_NETWORK_CHECK_IS_HOME;             //0;
            } 
            else if (response.endsWith(",5")) 
            {
              return GSM_NETWORK_CHECK_IS_ROAMING;          //1;
            }
            else if (response.endsWith(",8")) 
            {
              return GSM_NETWORK_CHECK_IS_EMERGENCY_ONLY;    //2;
            }
          }
        }

        delay(100);
      }

      return GSM_NETWORK_CHECK_IS_ERROR;    //-1;
    }
    
    void querySIMStatus()
    {
      send("AT+CLCK=\"SC\",2");
    }
    
    void lockSIM_PIN(String pin)
    {
      sendf("AT+CLCK=\"SC\",1,\"%s\"", pin.c_str());
    }
    
    void unlockSIM_PIN(String pin)
    {
      sendf("AT+CLCK=\"SC\",0,\"%s\"", pin.c_str());
    }
    
    void checkRegStatus()
    {
      // Network registration status +CREG
      send("AT+CREG?");
    }
    
    int unlockSim(String pin) 
    {
      sendf("AT+CPIN=\"%s\"", pin.c_str());
      
      if (waitForResponse(10000) == GSM_RESPONSE_OK) 
      {
        return SIM_PIN_UNLOCKED;
      }
      
      return checkSimStatus();
    }
    
    int checkSimStatus() 
    {     
      for (unsigned long start = millis(); (millis() - start) < 1000;) 
      {
        send("AT+CPIN?");
        
        String response;
              
        if (waitForResponse(10000, &response) == GSM_RESPONSE_OK) 
        {
          if (response.startsWith("+CPIN: ")) 
          {
            if (response.endsWith("READY")) 
            {
              return SIM_PIN_UNLOCKED;
            } 
            else if (response.endsWith("SIM PIN")) 
            {
              return SIM_PIN_NEED_PIN;
            } 
            else if (response.endsWith("SIM PUK")) 
            {
              return SIM_PIN_NEED_PUK;
            } 
          } 
        }
        
        return SIM_PIN_ERROR;
      }
      
      return SIM_PIN_ERROR;
    }
    
    // Write phonebook entry +CPBW
    /* Stores the phonebook entry in the current phonebook memory storage (selectable with +CPBS) at the location
       specified by the <index> field. Other entry fields are:
       • the phone number <number> (in the <type> format)
       • <text> text associated with the number
       • <group> indicating a group the entry may belong to
       • <adnumber> an additional number (of format <adtype>)
       • <secondtext> a second text field associated with the number
       • <email> an email field
       AT+CPBW=[<index>][,<number>[,<type>[,<text>[,<group>[,<adnumber>[,<adtype>[,<secondtext>[,<email>[,<sip_uri>[,<tel_uri>[,<hidden>]]]]]]]]]]]
    */
    int writePhoneBook(uint8_t index, String number, String name)
    {
      if (checkSimStatus() == SIM_PIN_UNLOCKED) 
      {
        sendf("AT+CPBW=%d,\"%s\",,\"%s\"", index, number.c_str(), name.c_str() );
        
        String response;
        
        if (waitForResponse(10000, &response) == GSM_RESPONSE_OK) 
        {
          if (response.lastIndexOf("OK\r\n"))
          {
            return SIM_PIN_WRITE_SUCCESS;
          } 
        }
        
        return SIM_PIN_WRITE_ERROR;
      } 
      else 
      {
        return SIM_PIN_WRITE_WRONG_PIN;
      }
    }
    
    // Read phonebook entries +CPBR
    /* Returns phonebook entries in location number range <index1> ... <index2> from the current phonebook memory
        storage selected with +CPBS. If <index2> is left out, only location <index1> is returned. Entry fields returned are:
        • location number <indexn>
        • phone number stored there <number> of format <type>
        • text <text> associated with the number
        • <group> indicating a group the entry may belong to (if the selected phonebook supports it)
        • <adnumber> an additional number (of format <adtype>) (if the selected phonebook supports it)
        • <secondtext> a second text field associated with the number (if the selected phonebook supports it)
        • <email> an email field (if the selected phonebook supports it)
        AT+CPBR=<index1>[,<index2>]
        [+CPBR: <index1>,<number>,<type>,<text>[,<group>[,<adnumber>[,<adtype>[,<secondtext>[,<email>[,<sip_uri>[,<tel_uri>]]]]]
        +CPBR: 1,"040123456",129,"RossiCarlo"
    */
    String readPhoneBook(uint8_t index, String& response)
    {     
      if (checkSimStatus() == SIM_PIN_UNLOCKED) 
      {
        sendf("AT+CPBR=%d", index);
               
        if (waitForResponse(10000, &response) == GSM_RESPONSE_OK) 
        {
          return response;
        }
      } 
      
      return String();
    }
    
    /////////////////////////////////////////
    //From GSMScanner_Generic_Impl.hpp
    
    void getCurrentCarrier()
    {
      send("AT+COPS?");
    }
    
    void testCurrentCarrier()
    {
      send("AT+COPS=?");
    }
    
    void getSignalStrength()
    {
      send("AT+CSQ");
    }
    
    void deregisterCarrier()
    {
      send("AT+COPS=2");
    }
    
    /////////////////////////////////////////
    //From GSM_SMS_Generic_Impl.hpp
    
    void sendSMS_To(const char* destNo)
    {
      sendf("AT+CMGS=\"%s\"", destNo);
    }
    
    void receivedUnreadSMS()
    {
      send("AT+CMGL=\"REC UNREAD\"");
    }
    
    void deleteSMSAtIndex(const char* index)
    {
      sendf("AT+CMGD=%s", index);
    }
    
    int readySMS(GSM_SMS_Data& _smsData, String& _incomingBuffer)
    {
      int readySMS = ready();

      if (readySMS == 0)
      {
        return 0;
      }

      switch (_smsData.state)
      {
        case SMS_STATE_IDLE:
        default:
          {
            break;
          }

        case SMS_STATE_LIST_MESSAGES:
          {
            setResponseDataStorage(&_incomingBuffer);
            
            // List message +CMGL => received unread SMS messages
            receivedUnreadSMS();
            
            _smsData.state = SMS_STATE_WAIT_LIST_MESSAGES_RESPONSE;
            readySMS = 0;
            break;
          }

        case SMS_STATE_WAIT_LIST_MESSAGES_RESPONSE:
          {
            _smsData.state = SMS_STATE_IDLE;
            break;
          }
      }

      return readySMS;
    }
    
    int availableSMS(GSM_SMS_Data& _smsData, String& _incomingBuffer)
    {
      if (_incomingBuffer.length() != 0) 
      {
        int nextMessageIndex = _incomingBuffer.indexOf("\r\n+CMGL: ");

        if (nextMessageIndex != -1) 
        {
          _incomingBuffer.remove(0, nextMessageIndex + 2);
        } 
        else 
        {
          _incomingBuffer = "";
        }
      }

      if (_incomingBuffer.length() == 0) 
      {
        int r;

        if (_smsData.state == SMS_STATE_IDLE) 
        {
          _smsData.state = SMS_STATE_LIST_MESSAGES;
        }

        if (_smsData.synch) 
        {
          while ((r = readySMS(_smsData, _incomingBuffer)) == 0)
          {
            delay(100);
          }
        } 
        else 
        {
          r = readySMS(_smsData, _incomingBuffer);
        }

        if (r != 1) 
        {
          return 0;
        }
      }

      if (_incomingBuffer.startsWith("+CMGL: ")) 
      {
        _smsData.smsDataIndex = _incomingBuffer.indexOf('\n') + 1;

        _smsData.smsDataEndIndex = _incomingBuffer.indexOf("\r\n+CMGL: ");
        
        if (_smsData.smsDataEndIndex == -1) 
        {
          _smsData.smsDataEndIndex = _incomingBuffer.length() - 1;
        }

        return (_smsData.smsDataEndIndex - _smsData.smsDataIndex) + 1;
      } 
      else 
      {
        _incomingBuffer = "";
      }

      return 0;
    }
    
    /////////////////////////////////////////
    //From GSMSSLClient_Generic_Impl.hpp
    
    void loadRootCert(const char* name, const int size)
    {
      sendf("AT+USECMNG=0,0,\"%s\",%d", name, size);
    }
    
    void removeRootCert(const char* name)
    {
      sendf("AT+USECMNG=2,0,\"%s\"", name);
    }
    
    void eraseTrustedRoot(const char* name) 
    {
      // SSL/TLS certificates and private keys manager +USECMNG
      sendf("AT+USECMNG=2,0,\"%s\"", name);
    }
    
    /////////////////////////////////////////
    //From GSMVoiceCall_Generic_Impl.hpp
    
    void dialNumber(const char* number) 
    {
      // Dial command ATD
      sendf("ATD%s;", number);
    }
    
    void answerCall() 
    {
      // Answer Incoming Call
      send("ATA");
    }
    
    void hangupCall() 
    {
      // Hangup
      send("ATH");
    }
    
    void retrieveCallingNumber() 
    {
      // List current calls +CLCC
      send("AT+CLCC");
    }
    
    void setSampleRate(int sampleRateNumber)
    {
      sendf("AT+UI2S=11,1,0,%d,1", sampleRateNumber);
    }
    
    void enableAudioPathNoHeadset()
    {
      // enable
      // Audio path mode setting +USPM
      // <main_uplink>, <main_downlink>, <alert_sound>, <headset_indication>, <vmic_ctrl>
      send("AT+USPM=1,1,0,0,2");
    }
    
    void disableAudioPathNoHeadset()
    {
      // disable
      // Audio path mode setting +USPM
      // <main_uplink>, <main_downlink>, <alert_sound>, <headset_indication>, <vmic_ctrl>
      send("AT+USPM=255,255,0,0,2");
    }
    
    void setDTMF(char c)
    {
      // DTMF and tone generation +VTS
      // <DTMF>
      sendf("AT+VTS=%c", c);
    }
    
    /////////////////////////////////////////
    //From utility/GSMSocketBuffer_Generic_Impl.hpp
    
    void readSocketData(int socket, int size)
    {
      sendf("AT+USORD=%d,%d", socket, size);
    }
    
    int availableSocketBuffer(int socket, SocketBufferList& buffers)
    {
      if (buffers.length == 0) 
      {
        if (buffers.data == NULL) 
        {
          buffers.data = buffers.head = (uint8_t*) malloc(GSM_SOCKET_BUFFER_SIZE);
          buffers.length = 0;
        }

        String response;

        //Read Socket Data +USORD   
        //sendf("AT+USORD=%d,%d", socket, GSM_SOCKET_BUFFER_SIZE);
        readSocketData(socket, GSM_SOCKET_BUFFER_SIZE);
        
        int status = waitForResponse(10000, &response);
        
        if (status != GSM_RESPONSE_OK) 
        {
          return -1;
        }

        if (!response.startsWith("+USORD: ")) 
        {
          return 0;
        }

        int firstQuoteIndex = response.indexOf("\"");

        response.remove(0, firstQuoteIndex + 1);
        response.remove(response.length() - 1);

        size_t size = response.length() / 2;

        for (size_t i = 0; i < size; i++) 
        {
          byte n1 = response[i * 2];
          byte n2 = response[i * 2 + 1];
          
          n1 = charToInt(n1);
          n2 = charToInt(n2);

          buffers.data[i] = (n1 << 4) | n2;
        }

        buffers.head = buffers.data;
        buffers.length = size;
      }

      return buffers.length;
    }
    
    /////////////////////////////////////////
    //From GSMModem_Generic_Impl.hpp
    
    String getIMEI()
    {
      String imei;

      imei.reserve(15);

      send("AT+CGSN");
      waitForResponse(100, &imei);

      return imei;
    }

    String getICCID()
    {
      String iccid;

      iccid.reserve(7 + 20);

      send("AT+CCID");
      waitForResponse(1000, &iccid);

      if (iccid.startsWith("+CCID: ")) 
      {
        iccid.remove(0, 7);
      } 
      else 
      {
        iccid = "";
      }

      return iccid;
    }
    
    // Asks for International Mobile Subscriber Identity IMSI via the AT+CIMI
    // Asks for International Mobile Subscriber Identity IMSI via the AT+CIMI
    String getIMSI()
    {
      String imsi;

      imsi.reserve(30);

      send("AT+CIMI");
      waitForResponse(1000, &imsi);

      // Trim out the imsi header in case it is there
      imsi.replace("CIMI:", "");
      imsi.trim();

      return imsi;
    }
        
    /////////////////////////////////////////

};

//////////////////////////////////////////////////////

#if (UBLOX_USING_LOW_POWER_MODE && UBLOX_USING_RESET_PIN)
  ModemClass MODEM(SerialGSM, 115200, GSM_RESETN, GSM_DTR);
#elif UBLOX_USING_LOW_POWER_MODE
  ModemClass MODEM(SerialGSM, 115200, -1, GSM_DTR);
#elif UBLOX_USING_RESET_PIN
  ModemClass MODEM(SerialGSM, 115200, GSM_RESETN);
#else
  ModemClass MODEM(SerialGSM, 115200);
#endif


ModemUrcHandler* ModemClass::_urcHandlers[MAX_URC_HANDLERS] = { NULL };
Print* ModemClass::_debugPrint = NULL;

//////////////////////////////////////////////////////

#include "Modem_UBLOX_Extra_Generic.h"


//////////////////////////////////////////////////////

#endif    // _MODEM_UBLOX_INCLUDED_H
