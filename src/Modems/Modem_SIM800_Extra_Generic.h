/*********************************************************************************************************************************
  Modem_SIM800_Extra_Generic.h
  
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

#ifndef _MODEM_SIM800_EXTRA_INCLUDED_H
#define _MODEM_SIM800_EXTRA_INCLUDED_H

///////////////////////////////////////////////////////////////////////

class GPRS_ModemUrcHandler : public ModemUrcHandler 
{
  public:
  
    int ready()
    {
      int ready = MODEM.ready();

      if (ready == GSM_RESPONSE_IDLE)
      {
        return GSM_RESPONSE_IDLE;
      }

      switch (_state)
      {
        case GPRS_STATE_IDLE:
        default:
          {
            break;
          }
         
        case GPRS_STATE_ATTACH:
          {
            MODEM.send("AT+CGATT=1");  // Attach to GPRS
                
            _state = GPRS_STATE_WAIT_ATTACH_GPRS_RESPONSE;
            ready = GSM_RESPONSE_IDLE;

            break;
          }

        case GPRS_STATE_WAIT_ATTACH_GPRS_RESPONSE:
          {
            if (ready > GSM_RESPONSE_OK)
            {
              _state = GPRS_STATE_IDLE;
              _status = GSM_ERROR;
            }
            else
            {
              _state = GPRS_STATE_SET_MULTI_IP;
              ready = GSM_RESPONSE_IDLE;
            }

            break;
          }
          
        case GPRS_STATE_SET_MULTI_IP:
          {
            MODEM.send("AT+CIPMUX=1");  // Set to multi-IP
                
            _state = GPRS_STATE_WAIT_SET_MULTI_IP_RESPONSE;
            ready = GSM_RESPONSE_IDLE;

            break;
          }

        case GPRS_STATE_WAIT_SET_MULTI_IP_RESPONSE:
          {
            if (ready > GSM_RESPONSE_OK)
            {
              _state = GPRS_STATE_IDLE;
              _status = GSM_ERROR;
            }
            else
            {
              _state = GPRS_STATE_QUICK_SEND;
              ready = GSM_RESPONSE_IDLE;
            }

            break;
          }
          
        // Put in "quick send" mode (thus no extra "Send OK")
        // Set to get data manually
        case GPRS_STATE_QUICK_SEND:
          {
            MODEM.send("AT+CIPQSEND=1");  // Put in "quick send" mode
                
            _state = GPRS_STATE_WAIT_QUICK_SEND_RESPONSE;
            ready = GSM_RESPONSE_IDLE;

            break;
          }

        case GPRS_STATE_WAIT_QUICK_SEND_RESPONSE:
          {
            if (ready > GSM_RESPONSE_OK)
            {
              _state = GPRS_STATE_IDLE;
              _status = GSM_ERROR;
            }
            else
            {
              _state = GPRS_STATE_START_TASK;
              ready = GSM_RESPONSE_IDLE;
            }

            break;
          }
          
        case GPRS_STATE_START_TASK:
          {
            // Start Task and Set APN, USER NAME, PASSWORD
            MODEM.sendf("AT+CSTT=\"%s\",\"%s\",\"%s\"", _apn, _username, _password);  
                
            _state = GPRS_STATE_WAIT_START_TASK_RESPONSE;
            ready = GSM_RESPONSE_IDLE;

            break;
          }

        case GPRS_STATE_WAIT_START_TASK_RESPONSE:
          {
            if (ready > GSM_RESPONSE_OK)
            {
              _state = GPRS_STATE_IDLE;
              _status = GSM_ERROR;
            }
            else
            {
              _state = GPRS_STATE_WIRELESS_UP;
              ready = GSM_RESPONSE_IDLE;
            }

            break;
          }
          
          case GPRS_STATE_WIRELESS_UP:
          {
            MODEM.send("AT+CIICR");  // Bring Up Wireless Connection with GPRS or CSD
                
            _state = GPRS_STATE_WAIT_WIRELESS_UP_RESPONSE;
            ready = GSM_RESPONSE_IDLE;

            break;
          }

        case GPRS_STATE_WAIT_WIRELESS_UP_RESPONSE:
          {
            if (ready > GSM_RESPONSE_OK)
            {
              _state = GPRS_STATE_IDLE;
              _status = GSM_ERROR;
            }
            else
            {
              _state = GPRS_STATE_GET_IP;
              ready = GSM_RESPONSE_IDLE;
            }

            break;
          }
          
        case GPRS_STATE_GET_IP:
          {
            MODEM.send("AT+CIFSR;E0");
            _state = GPRS_STATE_WAIT_GET_IP_RESPONSE;
            ready = GSM_RESPONSE_IDLE;

            break;
          }

        case GPRS_STATE_WAIT_GET_IP_RESPONSE:
          {
            if (ready > GSM_RESPONSE_OK)
            {
              _state = GPRS_STATE_IDLE;
              _status = GSM_ERROR;
            }
            else
            {
              _state = GPRS_STATE_CONFIG_DNS;
              ready = GSM_RESPONSE_IDLE;
            }

            break;
          }
          
        case GPRS_STATE_CONFIG_DNS:
          {
            // Using Google DNS
            MODEM.send("AT+CDNSCFG=\"8.8.8.8\",\"8.8.4.4\"");
            _state = GPRS_STATE_WAIT_CONFIG_DNS_RESPONSE;
            ready = GSM_RESPONSE_IDLE;

            break;
          }

        case GPRS_STATE_WAIT_CONFIG_DNS_RESPONSE:
          {
            if (ready > GSM_RESPONSE_OK)
            {
              _state = GPRS_STATE_IDLE;
              _status = GSM_ERROR;
            }
            else
            {
              _state = GPRS_STATE_IDLE;
              _status = GPRS_READY;
            }

            break;
          }
                            
        case GPRS_STATE_DEACTIVATE_IP:
          {
            MODEM.send("AT+CIPSHUT");   // Shut the TCP/IP connection to close *all* open connections
            
            _state = GPRS_STATE_WAIT_DEACTIVATE_IP_RESPONSE;
            ready = GSM_RESPONSE_IDLE;

            break;
          }

        case GPRS_STATE_WAIT_DEACTIVATE_IP_RESPONSE:
          {
            if (ready > GSM_RESPONSE_OK)
            {
              _state = GPRS_STATE_IDLE;
              _status = GSM_ERROR;
            }
            else
            {
              _state = GPRS_STATE_DEATTACH;
              ready = GSM_RESPONSE_IDLE;
            }

            break;
          }

        case GPRS_STATE_DEATTACH:
          {
            MODEM.send("AT+CGATT=0");
            _state = GPRS_STATE_WAIT_DEATTACH_RESPONSE;
            ready = GSM_RESPONSE_IDLE;

            break;
          }

        case GPRS_STATE_WAIT_DEATTACH_RESPONSE:
          {
            if (ready > GSM_RESPONSE_OK)
            {
              _state = GPRS_STATE_IDLE;
              _status = GSM_ERROR;
            }
            else
            {
              _state = GPRS_STATE_IDLE;
              _status = GSM_IDLE;
            }

            break;
          }         
      }

      return ready;
    }
    
    //////////////////////////////////////////////////////

    IPAddress getIPAddress()
    {
      String response;

      MODEM.send("AT+CIFSR;E0");

      if (MODEM.waitForResponse(100, &response) == GSM_RESPONSE_OK)
      {
        response.replace(GSM_NL "OK" GSM_NL, "");
        response.replace(GSM_NL, "");
        response.trim();
        
        IPAddress ip;

        if (ip.fromString(response))
        {
          return ip;
        }     
      }

      return IPAddress(0, 0, 0, 0);
    }
    
    //////////////////////////////////////////////////////

    int hostByName(const char* hostname, IPAddress& result)
    {
      String response;

      MODEM.sendf("AT+CDNSGIP=0,\"%s\"", hostname);


      if (MODEM.waitForResponse(70000, &response) != GSM_RESPONSE_OK)
      {
        return 0;
      }

      // KH to check
      if (!response.startsWith("+CDNSGIP:1 \"") || !response.endsWith("\""))
      {
        return 0;
      }

      // KH to check
      response.remove(0, 10);
      response.remove(response.length() - 1);

      if (result.fromString(response))
      {
        return 1;
      }

      return 0;
    }
    
    //////////////////////////////////////////////////////

    int ping(const char* hostname, uint8_t ttl)
    {
      String response;
      
      GSM_LOGDEBUG3(F("GPRS ping: UART hostname = "), hostname, ", ttl = ", ttl); 

      _pingResult = 0;
      
      // SIM800/900 => AT+CIPPING=<IPaddr>[,<retryNum>[,<dataLen>[,<timeout>[,<ttl>]]]]
      MODEM.sendf("AT+CIPPING=\"%s\",1,32,200,%d", hostname, ttl);
      
      //GSM_UNUSED(ttl);
      //MODEM.sendf("AT+CIPPING=\"%s\"", hostname);
      
      if (MODEM.waitForResponse() != GSM_RESPONSE_OK)
      {
        return GPRS_PING_ERROR;
      };

      for (unsigned long start = millis(); (millis() - start) < 5000 && (_pingResult == 0);)
      {
        MODEM.poll();
      }

      if (_pingResult == 0)
      {
        _pingResult = GPRS_PING_TIMEOUT;
      }

      return _pingResult;
    }
    
    //////////////////////////////////////////////////////

    void handleUrc(const String& urc)
    {
      // +CIPPING : <replyId>,<Ip Address>,<replyTime>,<ttl>
      
      if (urc.startsWith("+CIPPING: "))
      {
        int lastCommaIndex = urc.lastIndexOf(',');

        // No comma => error
        if (lastCommaIndex == -1)
        {
          _pingResult = GPRS_PING_ERROR;
        }
        else
        {        
          // URC
          // +CIPPING : <replyId>,<Ip Address>,<replyTime>,<ttl>
          String result;
          
          result = substringAfterSymbol(urc, ',');                          // Skip <replyId>
          
          GSM_LOGDEBUG1(F("GPRS::handleUrc: Receive PING from IPAddress = "), substringBeforeSymbol(result, ',') ); 
                        
          result = substringAfterSymbol(result, ',');                       // Skip <Ip Address>                      
            
          _pingResult = substringBeforeSymbol(result, ',').toInt();         // Get <replyTime>       
        }
      }
      else if ( urc.endsWith("ERROR") || urc.startsWith("+CME ERROR: ") )
      {
        _pingResult = GPRS_PING_ERROR;
      }  
    }
    
    protected:
      // To be used in derived classes
      const char* _apn;
      const char* _username;
      const char* _password;
      int         _state;
      
      GSM3_NetworkStatus_t  _status;
      String                _response;
      int                   _pingResult;
};

///////////////////////////////////////////////////////////////////////

class GSMClient_ModemUrcHandler : public ModemUrcHandler 
{
  public:
  
    virtual int ready()
    {
      int ready = MODEM.ready();

      if (ready == GSM_RESPONSE_IDLE)
      {
        return GSM_RESPONSE_IDLE;
      }

      switch (_state)
      {
        case CLIENT_STATE_IDLE:
        default:
          {
            break;
          }

        case CLIENT_STATE_CREATE_SOCKET:
          {
            MODEM.setResponseDataStorage(&_response);
            
            _state = CLIENT_STATE_CONNECT;
            ready = GSM_RESPONSE_IDLE;

            break;
          }

        case CLIENT_STATE_WAIT_CREATE_SOCKET_RESPONSE:
          {
            if (ready > GSM_RESPONSE_OK || !_response.startsWith("+USOCR: "))
            {
              _state = CLIENT_STATE_IDLE;
            }
            else
            {
              _socket = _response.charAt(_response.length() - 1) - '0';

              if (_ssl)
              {
                _state = CLIENT_STATE_ENABLE_SSL;
              }
              else
              {
                _state = CLIENT_STATE_CONNECT;
              }

              ready = GSM_RESPONSE_IDLE;
            }

            break;
          }

#if !(GSM_MODEM_SIM900 || TINY_GSM_MODEM_SIM900)
        // TO DO SSL (not supported for SIM900)
        case CLIENT_STATE_ENABLE_SSL:
          {
            //MODEM.sendf("AT+USOSEC=%d,1,0", _socket);

            _state = CLIENT_STATE_WAIT_ENABLE_SSL_RESPONSE;
            ready = GSM_RESPONSE_IDLE;

            break;
          }

        case CLIENT_STATE_WAIT_ENABLE_SSL_RESPONSE:
          {
            if (ready > GSM_RESPONSE_OK)
            {
              _state = CLIENT_STATE_CLOSE_SOCKET;
            }
            else
            {
              _state = CLIENT_STATE_MANAGE_SSL_PROFILE;
            }

            ready = GSM_RESPONSE_IDLE;

            break;
          }

        case CLIENT_STATE_MANAGE_SSL_PROFILE:
          {
            //MODEM.sendf("AT+USECPRF=0,0,%d", _sslprofile);

            _state = CLIENT_STATE_WAIT_MANAGE_SSL_PROFILE_RESPONSE;
            ready = GSM_RESPONSE_IDLE;

            break;
          }

        case CLIENT_STATE_WAIT_MANAGE_SSL_PROFILE_RESPONSE:
          {
            if (ready > GSM_RESPONSE_OK)
            {
              _state = CLIENT_STATE_CLOSE_SOCKET;
            }
            else
            {
              _state = CLIENT_STATE_CONNECT;
            }

            ready = GSM_RESPONSE_IDLE;

            break;
          }
#endif          
        //////////////////////////////////////////   

        case CLIENT_STATE_CONNECT:
          {
            if (_host != NULL)
            {
              // KH test, hardcode _socket
              _socket = 1;
              MODEM.sendf("AT+CIPSTART=%d,\"TCP\",\"%s\",%d", _socket, _host, _port);
            }
            else
            {
              MODEM.sendf("AT+CIPSTART=%d,\"%d.%d.%d.%d\",%d", _socket, _ip[0], _ip[1], _ip[2], _ip[3], _port);
            }

            _state = CLIENT_STATE_WAIT_CONNECT_RESPONSE;
            ready = GSM_RESPONSE_IDLE;

            break;
          }

        case CLIENT_STATE_WAIT_CONNECT_RESPONSE:
          {
            if (ready > GSM_RESPONSE_OK)
            {
              _state = CLIENT_STATE_CLOSE_SOCKET;

              ready = GSM_RESPONSE_IDLE;
            }
            else
            {
              _connected = true;
              _state = CLIENT_STATE_IDLE;
            }

            break;
          }

        case CLIENT_STATE_CLOSE_SOCKET:
          {
            MODEM.closeSocket(_socket);

            _state = CLIENT_STATE_WAIT_CLOSE_SOCKET;
            ready = GSM_RESPONSE_IDLE;

            break;
          }

        case CLIENT_STATE_WAIT_CLOSE_SOCKET:
          {
            _state = CLIENT_STATE_IDLE;
            _socket = -1;

            break;
          }
      }

      return ready;
    }
    
    //////////////////////////////////////////////////////
    
    size_t write(const uint8_t* buf, size_t size)
    {
      if (_writeSync)
      {
        while (ready() == GSM_RESPONSE_IDLE);
      }
      else if (ready() == GSM_RESPONSE_IDLE)
      {
        return 0;
      }

      if (_socket == -1)
      {
        return 0;
      }

      size_t written = 0;
      String command;

      command.reserve(19 + (size > 256 ? 256 : size) * 2);
      
      GSM_LOGDEBUG3(F("write: buf = "), (char*) buf, ", size = ", size);

      while (size)
      {
        //////
        
        size_t chunkSize = (size > 256) ? 256 : size;

        command = "AT+CIPSEND=";
        command += _socket;
        command += ",";
        command += chunkSize;
        
        GSM_LOGDEBUG1(F("write: AT command = "), command);

        MODEM.send(command);
        
        MODEM.waitForResponse(100);
        
        command = "\"";
        
        for (size_t i = 0; i < chunkSize; i++)
        {
          command += (char) buf[i + written];
        }

        command += "\"";
        
        GSM_LOGDEBUG1(F("write: AT data = "), command);

        MODEM.send(command);
        
        //////

        if (_writeSync)
        {
          if (MODEM.waitForResponse(10000) != GSM_RESPONSE_OK)
          {
            break;
          }
        }

        written += chunkSize;
        size -= chunkSize;
        
        GSM_LOGDEBUG3(F("write: written = "), written, ", remaining size = ", size);
      }

      return written;
    }
    
    //////////////////////////////////////////////////////
   
    virtual void handleUrc(const String& urc)
    {
      GSM_LOGDEBUG1(F("GSMClient::handleUrc: urc = "), urc);
      
      // URC
      //+CIPRXGET: 2,<id>,<reqlength>,<cnflength>[,<IPADDRESS>:<PORT>]
      //1234567890...
      if (urc.startsWith("+CIPRXGET: "))
      {
        // KH to fix here
        int socket = urc.charAt(9) - '0';

        if (socket == _socket)
        {
          if (urc.endsWith(",4294967295"))
          {
            _connected = false;
          }
        }
        //////
      }
    }
    
    //////////////////////////////////////////////////////  
    
  protected:
    // To be used in derived classes
    int         _socket;
    int         _connected;

    int         _state;
    IPAddress   _ip;
    const char* _host;
    uint16_t    _port;
    bool        _ssl;

    int         _sslprofile;
    bool        _writeSync;
    String      _response;
};

///////////////////////////////////////////////////////////////////////

#include "GSMClient_Generic.h"

#define MAX_CHILD_SOCKETS 6

class GSMServer_ModemUrcHandler : public ModemUrcHandler 
{
  public:
  
    int ready()
    {
      int ready = MODEM.ready();

      if (ready == GSM_RESPONSE_IDLE)
      {
        return GSM_RESPONSE_IDLE;
      }

      switch (_state)
      {
        case SERVER_STATE_IDLE:
        default:
          {
            break;
          }

        case SERVER_STATE_CREATE_SOCKET:
          {
            MODEM.setResponseDataStorage(&_response);
            
            // 6: Create TCP socket
            //MODEM.send("AT+USOCR=6");

            _state = SERVER_STATE_WAIT_CREATE_SOCKET_RESPONSE;
            ready = GSM_RESPONSE_IDLE;

            break;
          }

        case SERVER_STATE_WAIT_CREATE_SOCKET_RESPONSE:
          {
            if (ready > GSM_RESPONSE_OK || !_response.startsWith("+USOCR: "))
            {
              _state = SERVER_STATE_IDLE;
            }
            else
            {
              _socket = _response.charAt(_response.length() - 1) - '0';

              _state = SERVER_STATE_LISTEN;
              ready = GSM_RESPONSE_IDLE;
            }

            break;
          }

        case SERVER_STATE_LISTEN:
          {
            MODEM.sendf("AT+USOLI=%d,%d", _socket, _port);

            _state = SERVER_STATE_WAIT_LISTEN_RESPONSE;
            ready = GSM_RESPONSE_IDLE;

            break;
          }

        case SERVER_STATE_WAIT_LISTEN_RESPONSE:
          {
            if (ready > GSM_RESPONSE_OK)
            {
              _state = SERVER_STATE_CLOSE_SOCKET;
              ready = GSM_RESPONSE_IDLE;
            }
            else
            {
              _state = SERVER_STATE_IDLE;
            }

            break;
          }

        case SERVER_STATE_CLOSE_SOCKET:
          {
            MODEM.closeSocket(_socket);

            _state = SERVER_STATE_WAIT_CLOSE_SOCKET;
            ready = GSM_RESPONSE_IDLE;

            break;
          }

        case SERVER_STATE_WAIT_CLOSE_SOCKET:
          {
            _state = SERVER_STATE_IDLE;
            _socket = -1;
            break;
          }
      }

      return ready;
    }
    
    //////////////////////////////////////////////////////
    
    GSMClient available(bool synch = true)
    {
      MODEM.poll();

      int socket = -1;

      if (_socket != -1) 
      {
        // search for new accepted sockets first
        for (int i = 0; i < MAX_CHILD_SOCKETS; i++) 
        {
          if (_childSockets[i].socket != -1 && _childSockets[i].accepted) 
          {
            _childSockets[i].accepted = false;
            socket = _childSockets[i].socket;

            break;
          }
        }

        if (socket == -1) 
        {
          // no new accepted sockets, search for one with data to be read
          for (int i = 0; i < MAX_CHILD_SOCKETS; i++) 
          {
            if (_childSockets[i].socket != -1) 
            {
              // check if socket is still alive
              //MODEM.sendf("AT+USORD=%d,0", _childSockets[i].socket);
              MODEM.sendf("AT+CIPRXGET=2,%d,0", _childSockets[i].socket);
              
              if (MODEM.waitForResponse(10000) != GSM_RESPONSE_OK) 
              {
                // closed
                _childSockets[i].socket = -1;
                _childSockets[i].accepted = false;
                _childSockets[i].available = 0;

                continue;
              }

              if (_childSockets[i].available) 
              {
                _childSockets[i].available = 0;
                socket = _childSockets[i].socket;
                break;
              }
            }
          }
        }
      }

      return GSMClient(socket, synch);
    }
    
    ////////////////////////////////////////////////////// 
    
    void stop()
    {
      if (_socket < 0) 
      {
        return;
      }

      MODEM.closeSocket(_socket);
      
      MODEM.waitForResponse(10000);

      _socket = -1;
    }
    
    //////////////////////////////////////////////////////  
  
    virtual void handleUrc(const String& urc)
    {
      GSM_LOGDEBUG1(F("GSMServer::handleUrc: urc = "), urc);
      
      if (urc.startsWith("+UUSOLI: ")) 
      {
        int socket = urc.charAt(9) - '0';

        for (int i = 0; i < MAX_CHILD_SOCKETS; i++) 
        {
          if (_childSockets[i].socket == -1) 
          {
            _childSockets[i].socket = socket;
            _childSockets[i].accepted = true;
            _childSockets[i].available = 0;

            break;
          }
        }
      } 
      else if (urc.startsWith("+UUSOCL: ")) 
      {
        int socket = urc.charAt(urc.length() - 1) - '0';

        if (socket == _socket) 
        {
          _socket = -1;
        } 
        else 
        {
          for (int i = 0; i < MAX_CHILD_SOCKETS; i++) 
          {
            if (_childSockets[i].socket == socket) 
            {
              _childSockets[i].socket = -1;
              _childSockets[i].accepted = false;
              _childSockets[i].available = 0;

              break;
            }
          }
        }
      } 
      //else if (urc.startsWith("+UUSORD: ")) 
      else if (urc.startsWith("+CIPRXGET: "))
      {
        // KH to fix here
        // URC
        //+CIPRXGET: 2,<id>,<reqlength>,<cnflength>[,<IPADDRESS>:<PORT>]
        //1234567890...
        int socket = urc.charAt(9) - '0';
        // KH to fix here
        
        for (int i = 0; i < MAX_CHILD_SOCKETS; i++) 
        {
          if (_childSockets[i].socket == socket) 
          {
            int commaIndex = urc.indexOf(',');
            
            if (commaIndex != -1) 
            {
              _childSockets[i].available = urc.substring(commaIndex + 1).toInt();
            }

            break;
          }
        }
      }
    }
    
    //////////////////////////////////////////////////////  
    
  protected:
    uint16_t  _port;
    bool      _synch;

    int       _socket;
    int       _state;
    String    _response;

    struct 
    {
      int socket;
      bool accepted;
      int available;
    } _childSockets[MAX_CHILD_SOCKETS];
};

///////////////////////////////////////////////////////////////////////

class GSMUDP_ModemUrcHandler : public ModemUrcHandler 
{
  public:
  
    uint8_t begin(uint16_t port)
    {     
      GSM_LOGDEBUG1(F("GSMUDP::begin: port = "), port);
    
      return (begin(IPAddress(0,0,0,0), port));
    }
    
    uint8_t begin(IPAddress ip, uint16_t port)
    {      
      GSM_LOGDEBUG3(F("GSMUDP::begin: IP = "), ip, F(", port = "), port);
      
      _txIp = ip;
      _txHost = NULL;
      _txPort = port;

      // KH test, hardcode _socket
      _socket = 1;

      MODEM.sendf("AT+CIPSTART=%d,\"UDP\",\"%d.%d.%d.%d\",%d", _socket, _txIp[0], _txIp[1], _txIp[2], _txIp[3], _txPort);
      
      if (MODEM.waitForResponse() != GSM_RESPONSE_OK)
      {
        GSM_LOGDEBUG(F("GSMUDP::begin: Error no modem response"));
        
        return 0;
      }
     
      GSM_LOGDEBUG1(F("GSMUDP::begin: OK, _socket = "), _socket);

      return 1;
    }
    
    
    uint8_t begin(const char *host, uint16_t port)
    {
      GSM_LOGDEBUG3(F("GSMUDP::begin: host = "), host, F(", port = "), port);
      
      _txIp = IPAddress(0,0,0,0);     //(uint32_t)0;
      _txHost = host;
      _txPort = port;
      
      // KH test, hardcode _socket
      _socket = 1;

      MODEM.sendf("AT+CIPSTART=%d,\"UDP\",\"%s\",%d", _socket, _txHost, _txPort);

      if (MODEM.waitForResponse() != GSM_RESPONSE_OK)
      {
        GSM_LOGDEBUG(F("GSMUDP::begin: Error no modem response"));
        
        return 0;
      }
      
      GSM_LOGDEBUG1(F("GSMUDP::begin: OK, _socket = "), _socket);

      return 1;
    }
    ////////////////////////////////////////////////////// 

    void stop()
    {
      if (_socket < 0)
      {
        return;
      }

      MODEM.closeSocket(_socket);
      
      MODEM.waitForResponse(10000);

      _socket = -1;
    }
    
    ////////////////////////////////////////////////////// 
    
    int endPacket()
    {
      String command;

      if (_txHost != NULL)
      {
        command.reserve(26 + strlen(_txHost) + _txSize * 2);
      }
      else
      {
        command.reserve(41 + _txSize * 2);
      }

#if 1
        command = "AT+CIPSEND=";
        command += _socket;
        command += ",";
        command += _txSize;
        
        GSM_LOGDEBUG1(F("write: AT command = "), command);

        MODEM.send(command);
        
        MODEM.waitForResponse(100);
        
        //command = "\"";
        command = "";
        
        for (size_t i = 0; i < _txSize; i++)
        {
          command += (char) _txBuffer[i];
        }

        //command += "\"";
        
        GSM_LOGDEBUG1(F("write: AT command = "), command);

        MODEM.send(command);
#else
      command = "AT+CIPSEND=";
      command += _socket;
      command += ",\"";

      if (_txHost != NULL)
      {
        command += _txHost;
      }
      else
      {
        command += _txIp[0];
        command += '.';
        command += _txIp[1];
        command += '.';
        command += _txIp[2];
        command += '.';
        command += _txIp[3];
      }

      command += "\",";
      command += _txPort;
      command += ",";
      command += _txSize;
      command += ",\"";
      
      for (size_t i = 0; i < _txSize; i++)
      {
        command += (char) _txBuffer[i];
      }
      
      command += "\"";
      
      GSM_LOGDEBUG1(F("UDP endPacket: AT command = "), command);

      MODEM.send(command);
#endif

      if (MODEM.waitForResponse() == GSM_RESPONSE_OK) 
      {
        return 1;
      } 
      else 
      {
        return 0;
      }
    }
    
    ////////////////////////////////////////////////////// 
    
    int parsePacket()
    {
      MODEM.poll();

      if (_socket < 0)
      {
        return 0;
      }

      if (!_packetReceived) 
      {
        return 0;
      }
      
      _packetReceived = false;
      
      //KH to fix

      String response;

      // AT+CIPRXGET=<mode>[,<reqlength>]
      // Mode 2 ????
      MODEM.sendf("AT+CIPRXGET=%d,%d", 2 /*_socket*/, sizeof(_rxBuffer));
      
      if (MODEM.waitForResponse(10000, &response) != GSM_RESPONSE_OK) 
      {
        return 0;
      }

      if (!response.startsWith("+USORF: ")) 
      {
        return 0;
      }
      
      // URC
      //+CIPRXGET: 2,<mux_id>,<reqlength>,<cnflength>[,<IPADDRESS>:<PORT>]
      //1234567890...
      if (response.startsWith("+CIPRXGET: "))
      {
        String result;
        
        result = substringAfterSymbol(response, ',');                     // Skip Rx mode
        result = substringAfterSymbol(result, ',');                       // Skip <mux_id>       
          
        int requestLength = substringBeforeSymbol(result, ',').toInt();   // Get <reqlength>
        
        result = substringAfterSymbol(result, ',');                       // Skip <reqlength>
        
        // KH to fix here. What we need to do ?
        _rxSize = substringBeforeSymbol(result, ',').toInt();
        
        result = response.substring(response.indexOf('\n') + 1);          // Get <data>
        
        GSM_LOGDEBUG3(F("GSMUDP::handleUrc: requestLength = "), requestLength, 
                      F(", confirmedLength _rxSize = "), _rxSize);
        GSM_LOGDEBUG1(F("GSMUDP::handleUrc: data = "), result);              
        
        //////
 
        _rxIndex = 0;
        
        memcpy(_rxBuffer, result.c_str(), _rxSize);
      }
      else
      {
        return 0;
      }
     
      ////////////////

      MODEM.poll();

      return _rxSize;
    }
       
    ////////////////////////////////////////////////////// 
  
    // URC - Unsolicited Response Code
    // The DCE may send responses back to the DTE indicating the outcome of the command or further 
    // information without having received any commands by the DTE

    //String substringAfterSymbol(String str, char symbol)
    //String substringBeforeSymbol(String str, char symbol)
    
    ////////////////////////////////////////////////////// 
    
    virtual void handleUrc(const String& urc)
    {
      GSM_LOGDEBUG1(F("GSMUDP::handleUrc: urc = "), urc);

      // URC
      //+CIPRXGET: 2,<mux_id>,<reqlength>,<cnflength>[,<IPADDRESS>:<PORT>]
      //1234567890...
      if (urc.startsWith("+CIPRXGET: "))
      {
        String result;
        
        result = substringAfterSymbol(urc, ',');                        // Skip Rx mode
        result = substringAfterSymbol(result, ',');                     // Skip <mux_id>       
        
        int requestLength = substringBeforeSymbol(result, ',').toInt(); // Get <reqlength>
        
        result = substringAfterSymbol(result, ',');                     // Skip <reqlength>
        
        // KH to fix here. What we need to do ?
        _rxSize = substringBeforeSymbol(result, ',').toInt();
        
        result = urc.substring(urc.indexOf('\n') + 1);                   // Get <data>
        
        GSM_LOGDEBUG3(F("GSMUDP::handleUrc: requestLength = "), requestLength, 
                      F(", confirmedLength _rxSize = "), _rxSize);
        GSM_LOGDEBUG1(F("GSMUDP::handleUrc: data = "), result);              
        
        //////
 
        _rxIndex = 0;
        
        memcpy(_rxBuffer, result.c_str(), _rxSize);
      }
      // Response to CLOSE OK from AT+CIPCLOSE to close TCP or UDP Connection
      else if (urc.startsWith("CLOSE OK")) 
      {
        // this socket closed
        //_socket = -1;
        _rxIndex = 0;
        _rxSize = 0;
      }    
    }
    
    ////////////////////////////////////////////////////// 
    
  protected:
    int         _socket;
    bool        _packetReceived;

    IPAddress   _txIp;
    const char* _txHost;
    uint16_t    _txPort;
    size_t      _txSize;
    uint8_t     _txBuffer[512];

    IPAddress   _rxIp;
    uint16_t    _rxPort;
    size_t      _rxSize;
    size_t      _rxIndex;
    uint8_t     _rxBuffer[512];
};

///////////////////////////////////////////////////////////////////////

class GSMLocation_ModemUrcHandler : public ModemUrcHandler 
{
  public:

    void handleUrc(const String& urc)
    {
      GSM_LOGDEBUG1(F("GSMLocation::handleUrc: urc = "), urc);
      
      if (urc.startsWith("+UULOC: ")) 
      {
        String temp = urc;
        int lastCommaIndex;

        _locationAvailable = true;

        lastCommaIndex = temp.lastIndexOf(',');
        _uncertainty = temp.substring(lastCommaIndex + 1).toInt();
        temp.remove(lastCommaIndex);

        lastCommaIndex = temp.lastIndexOf(',');
        _altitude = temp.substring(lastCommaIndex + 1).toInt();
        temp.remove(lastCommaIndex);

        lastCommaIndex = temp.lastIndexOf(',');
        _longitude = temp.substring(lastCommaIndex + 1).toFloat();
        temp.remove(lastCommaIndex);

        lastCommaIndex = temp.lastIndexOf(',');
        _latitude = temp.substring(lastCommaIndex + 1).toFloat();
        temp.remove(lastCommaIndex);
      }
    }
    
  protected:
  
    bool  _commandSent;
    bool  _locationAvailable;

    float _latitude;
    float _longitude;
    long  _altitude;
    long  _uncertainty;

};

///////////////////////////////////////////////////////////////////////


class GSMVoiceCall_ModemUrcHandler : public ModemUrcHandler 
{
  public:
    
    void handleUrc(const String& urc)
    {
      GSM_LOGDEBUG1(F("GSMVoiceCall::handleUrc: urc = "), urc);
      
      if (urc.startsWith("+UCALLSTAT: ")) 
      {
        int status = urc.charAt(urc.length() - 1) - '0';

        if (status == 0 || status == 1 || status == 7) 
        {
          _callStatus = TALKING;
          _dtmfBuffer = "";
          
          GSM_LOGDEBUG(F("GSMVoiceCall::handleUrc: TALKING"));
        } 
        else if (status == 2 || status == 3) 
        {
          _callStatus = CALLING;
          
          GSM_LOGDEBUG(F("GSMVoiceCall::handleUrc: CALLING"));
        } 
        else if (status == 4 || status == 5) 
        {
          _callStatus = RECEIVINGCALL;
          
          GSM_LOGDEBUG(F("GSMVoiceCall::handleUrc: RECEIVINGCALL"));
        } 
        else 
        {
          _callStatus = IDLE_CALL;
          
          GSM_LOGDEBUG(F("GSMVoiceCall::handleUrc: IDLE_CALL"));
        }
      } 
      else if (urc.startsWith("+UUDTMFD: ")) 
      {
        _dtmfBuffer += urc.charAt(10);
      }
    }
    
  protected:
    int               _synch;
    GSM3_voiceCall_st _callStatus;
    String            _dtmfBuffer; 
};


///////////////////////////////////////////////////////////////////////

#endif    // _MODEM_SIM800_EXTRA_INCLUDED_H
