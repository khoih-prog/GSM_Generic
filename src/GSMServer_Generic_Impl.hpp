/*********************************************************************************************************************************
  GSMServer_Generic_Impl.hpp
  
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

#ifndef _GSM_SERVER_HPP
#define _GSM_SERVER_HPP

GSMServer::GSMServer(uint16_t port, bool synch)
{
  _port   = port;
  _synch  = synch;
  _socket = -1;
  _state  = SERVER_STATE_IDLE;
  
  for (int i = 0; i < MAX_CHILD_SOCKETS; i++)
  {
    _childSockets[i].socket = -1;
    _childSockets[i].accepted = false;
    _childSockets[i].available = 0;
  }

  MODEM.addUrcHandler(this);
}

GSMServer::~GSMServer()
{
  MODEM.removeUrcHandler(this);
}

void GSMServer::begin()
{
  _state = SERVER_STATE_CREATE_SOCKET;

  if (_synch) 
  {
    while (ready() == 0);
  }
}

void GSMServer::beginWrite()
{
}

size_t GSMServer::write(uint8_t c)
{
  return write(&c, sizeof(c));
}

size_t GSMServer::write(const uint8_t *buf)
{
  return write(buf, strlen((const char*)buf));
}

size_t GSMServer::write(const uint8_t *buf, size_t sz)
{
  size_t written = 0;

  MODEM.poll();

  if (_socket != -1) 
  {
    for (int i = 0; i < MAX_CHILD_SOCKETS; i++) 
    {
      if (_childSockets[i].socket != -1) 
      {
        GSMClient client(_childSockets[i].socket, true);

        written += client.write(buf, sz);
      }
    }
  }

  return written;
}

void GSMServer::endWrite()
{
}

#endif    // _GSM_SERVER_HPP
