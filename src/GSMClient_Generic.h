/*********************************************************************************************************************************
  GSMClient_Generic.h
  
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

#ifndef _GSM_CLIENT_H_INCLUDED
#define _GSM_CLIENT_H_INCLUDED

#include "GSM_Generic_Debug.h"

#include <Client.h>

#include "GSM_Type_Generic.h"

#define GSMCLIENT_USING_NEW_CLASS      true

class GSMClient : public Client, public GSMClient_ModemUrcHandler
{
  public:

    /** Constructor
        @param synch    Sync mode
    */
    GSMClient(bool synch = true);

    /** Constructor
        @param socket   Socket
        @param synch    Sync mode
    */
    GSMClient(int socket, bool synch);

    virtual ~GSMClient();

    

    /** Connect to server by IP address
        @param (IPAddress)
        @param (uint16_t)
        @return returns 0 on failure, 1 on success (sync mode)
                        0 if last command is still executing, 1 success, 2 if there are no resources (async mode)   */
    int connect(IPAddress, uint16_t);
    int connectSSL(IPAddress, uint16_t);

    /** Connect to server by hostname
        @param host     Hostname
        @param port     Port
        @return returns 0 on failure, 1 on success (sync mode)
                        0 if last command is still executing, 1 success, 2 if there are no resources (async mode)
    */
    int connect(const char *host, uint16_t port);
    int connectSSL(const char *host, uint16_t port);

    /** Initialize write in request
        @param sync     Sync mode
    */
    void beginWrite(bool sync = false);

    /** Write a character in request
        @param c      Character
        @return size
    */
    size_t write(uint8_t c);

    /** Write a characters buffer in request
        @param buf      Buffer
        @return buffer size
    */
    size_t write(const uint8_t *buf);

    /** Write a characters buffer with size in request
        @param (uint8_t*) Buffer
        @param (size_t)   Buffer size
        @return buffer size
    */
    size_t write(const uint8_t*, size_t);

    /** Finish write request
        @param sync     Sync mode
    */
    void endWrite(bool sync = false);

    /** Check if connected to server
        @return 1 if connected
    */
    uint8_t connected();

    operator bool();

    /** Read from response buffer and copy size specified to buffer
        @param buf      Buffer
        @param size     Buffer size
        @return bytes read
    */
    int read(uint8_t *buf, size_t size);

    /** Read a character from response buffer
        @return character
    */
    int read();

    /** Check if exists a response available
        @return 1 if exists, 0 if not exists
    */
    int available();

    /** Read a character from response buffer but does not move the pointer.
        @return character
    */
    int peek();

    /** Flush response buffer
    */
    void flush();

    /** Stop client
    */
    void stop();

    /** Set ssl profile
    */
    void setCertificateValidationLevel(uint8_t ssl);

  private:
    int connect();

    bool        _synch;    
};

#include "GSMClient_Generic_Impl.hpp"

#endif    // _GSM_CLIENT_H_INCLUDED
