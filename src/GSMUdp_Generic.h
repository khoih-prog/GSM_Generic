/*********************************************************************************************************************************
  GSMUdp_Generic.h
  
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

#ifndef _GSM_UDP_H_INCLUDED
#define _GSM_UDP_H_INCLUDED

#include <Udp.h>

#include "GSM_Generic_Debug.h"

#include "GSM_Type_Generic.h"

class GSMUDP : public UDP, public GSMUDP_ModemUrcHandler
{
  public:
    GSMUDP();  // Constructor
    virtual ~GSMUDP();

    // initialize, start listening from specified Server UDP port. 
    // Returns 1 if successful, 0 if there are no sockets available to use
    virtual uint8_t begin(uint16_t port);

    //  KH add
    virtual uint8_t begin(IPAddress ip, uint16_t port);
    virtual uint8_t begin(const char *host, uint16_t port);
    //////
    
    virtual void stop();              // Finish with the UDP socket

    // Sending UDP packets

    // Start building up a packet to send to the remote host specific in ip and port
    // Returns 1 if successful, 0 if there was a problem with the supplied IP address or port
    virtual int beginPacket(IPAddress ip, uint16_t port);
    
    // Start building up a packet to send to the remote host specific in host and port
    // Returns 1 if successful, 0 if there was a problem resolving the hostname or port
    virtual int beginPacket(const char *host, uint16_t port);
    
    // Finish off this packet and send it
    // Returns 1 if the packet was sent successfully, 0 if there was an error
    virtual int endPacket();
    
    // Write a single byte into the packet
    virtual size_t write(uint8_t);
    
    // Write size bytes from buffer into the packet
    virtual size_t write(const uint8_t *buffer, size_t size);

    using Print::write;

    // Start processing the next available incoming packet
    // Returns the size of the packet in bytes, or 0 if no packets are available
    virtual int parsePacket();
    
    // Number of bytes remaining in the current packet
    virtual int available();
    
    // Read a single byte from the current packet
    virtual int read();
    
    // Read up to len bytes from the current packet and place them into buffer
    // Returns the number of bytes read, or 0 if none are available
    virtual int read(unsigned char* buffer, size_t len);
    
    // Read up to len characters from the current packet and place them into buffer
    // Returns the number of characters read, or 0 if none are available
    virtual int read(char* buffer, size_t len) 
    {
      return read((unsigned char*)buffer, len);
    };
    
    // Return the next byte from the current packet without moving on to the next byte
    virtual int peek();

    // Finish reading the current packet
    virtual void flush();

    // Return the IP address of the host who sent the current incoming packet
    virtual IPAddress remoteIP();
    
    // Return the port of the host who sent the current incoming packet
    virtual uint16_t remotePort();

  private:
    
};

#include "GSMUdp_Generic_Impl.hpp"

#endif    // _GSM_UDP_H_INCLUDED
