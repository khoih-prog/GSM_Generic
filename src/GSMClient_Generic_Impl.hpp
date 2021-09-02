/*********************************************************************************************************************************
  GSMClient_Generic_Impl.hpp
  
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

#ifndef _GSM_CLIENT_HPP
#define _GSM_CLIENT_HPP

#include "utility/GSMSocketBuffer_Generic.h"

//#include "GSMClient_Generic.h"

GSMClient::GSMClient(bool synch) :
  GSMClient(-1, synch)
{
}

GSMClient::GSMClient(int socket, bool synch)
{
  _synch          = synch;
   _socket        = socket;
  _connected      = false;
  _state          = CLIENT_STATE_IDLE;
  _ip             = (uint32_t) 0;
  _host           = NULL;
  _port           = 0;
  _ssl            = false;
  _sslprofile     = 1;
  _writeSync      = true;
  
  MODEM.addUrcHandler(this);
}

GSMClient::~GSMClient()
{
  MODEM.removeUrcHandler(this);
}

size_t GSMClient::write(const uint8_t* buf, size_t size)
{
  return GSMClient_ModemUrcHandler::write(buf, size);
}

int GSMClient::connect(IPAddress ip, uint16_t port)
{
  _ip = ip;
  _host = NULL;
  _port = port;
  _ssl = false;
  
  GSM_LOGDEBUG3(F("GSMClient::connect: IP = "), _ip, F(", port = "), _port);

  return connect();
}

int GSMClient::connectSSL(IPAddress ip, uint16_t port)
{
  _ip = ip;
  _host = NULL;
  _port = port;
  _ssl = true;

  GSM_LOGDEBUG3(F("GSMClient::connectSSL: IP = "), _ip, F(", port = "), _port);
  
  return connect();
}

int GSMClient::connect(const char *host, uint16_t port)
{
  _ip = (uint32_t)0;
  _host = host;
  _port = port;
  _ssl = false;
  
  GSM_LOGDEBUG3(F("GSMClient::connect: host = "), _host, F(", port = "), _port);

  return connect();
}

int GSMClient::connectSSL(const char *host, uint16_t port)
{
  _ip = (uint32_t)0;
  _host = host;
  _port = port;
  _ssl = true;
  
  GSM_LOGDEBUG3(F("GSMClient::connectSSL: host = "), _host, F(", port = "), _port);

  return connect();
}

int GSMClient::connect()
{
  if (_socket != -1)
  {
    GSM_LOGDEBUG(F("GSMClient::connect: socket existing error"));
    
    stop();
  }

  if (_synch)
  {
    while (ready() == 0);
  }
  else if (ready() == 0)
  {
    return 0;
  }

  _state = CLIENT_STATE_CREATE_SOCKET;

  if (_synch)
  {
    while (ready() == 0)
    {
      delay(100);
    }

    if (_socket == -1)
    {
      GSM_LOGDEBUG(F("GSMClient::connect: error no socket"));
      
      return 0;
    }
  }

  GSM_LOGDEBUG(F("GSMClient::connect: OK"));
  
  return 1;
}

void GSMClient::beginWrite(bool sync)
{
  _writeSync = sync;
}

size_t GSMClient::write(uint8_t c)
{
  return write(&c, 1);
}

size_t GSMClient::write(const uint8_t *buf)
{
  return write(buf, strlen((const char*)buf));
}

void GSMClient::endWrite(bool /*sync*/)
{
  _writeSync = true;
}

uint8_t GSMClient::connected()
{
  MODEM.poll();

  if (_socket == -1)
  {
    GSM_LOGDEBUG(F("GSMClient::connected: no socket"));
    
    return 0;
  }

  // call available to update socket state
  if ((GSMSocketBuffer.available(_socket) < 0) || (_ssl && !_connected))
  {
    stop();

    return 0;
  }
  
  GSM_LOGDEBUG(F("GSMClient::connected: OK"));

  return 1;
}

GSMClient::operator bool()
{
  return (_socket != -1);
}

int GSMClient::read(uint8_t *buf, size_t size)
{
  if (_socket == -1)
  {
    return 0;
  }

  if (size == 0)
  {
    return 0;
  }

  int avail = available();

  if (avail == 0)
  {
    return 0;
  }

  return GSMSocketBuffer.read(_socket, buf, size);
}

int GSMClient::read()
{
  byte b;

  if (read(&b, 1) == 1)
  {
    return b;
  }

  return -1;
}

int GSMClient::available()
{
  if (_synch)
  {
    while (ready() == 0);
  }
  else if (ready() == 0)
  {
    return 0;
  }

  if (_socket == -1)
  {
    return 0;
  }

  int avail = GSMSocketBuffer.available(_socket);

  if (avail < 0)
  {
    stop();

    return 0;
  }

  return avail;
}

int GSMClient::peek()
{
  if (available() > 0)
  {
    return GSMSocketBuffer.peek(_socket);
  }

  return -1;
}

void GSMClient::flush()
{
}

void GSMClient::stop()
{
  _state = CLIENT_STATE_IDLE;

  if (_socket < 0)
  {
    return;
  }

  MODEM.closeSocket(_socket);
        
  MODEM.waitForResponse(10000);

  GSMSocketBuffer.close(_socket);
  _socket = -1;
  _connected = false;
}

void GSMClient::setCertificateValidationLevel(uint8_t ssl)
{
  _sslprofile = ssl;
}

#endif    // _GSM_CLIENT_HPP
