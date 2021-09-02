/*********************************************************************************************************************************
  GSMUdp_Generic_Impl.hpp
  
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

#ifndef _GSM_UDP_IMPL_HPP
#define _GSM_UDP_IMPL_HPP

//#include "GSMUdp_Generic.h"

GSMUDP::GSMUDP()
{
  _socket           = -1;
  _packetReceived   = false;
  _txIp             = (uint32_t) 0;
  _txHost           = NULL;
  _txPort           = 0;
  _txSize           = 0;
  _rxIp             = (uint32_t) 0;
  _rxPort           = 0;
  _rxSize           = 0;
  _rxIndex          = 0;
  
  MODEM.addUrcHandler(this);
}

GSMUDP::~GSMUDP()
{
  MODEM.removeUrcHandler(this);
}

uint8_t GSMUDP::begin(uint16_t port)
{
  return GSMUDP_ModemUrcHandler::begin(port);
}

//////////////////////////////////////////////////////
//  KH add

uint8_t GSMUDP::begin(IPAddress ip, uint16_t port)
{
  return GSMUDP_ModemUrcHandler::begin(ip, port);
  //return GSMUDP_ModemUrcHandler::begin(port);
}

uint8_t GSMUDP::begin(const char *host, uint16_t port)
{
  return GSMUDP_ModemUrcHandler::begin(host, port);
  //return GSMUDP_ModemUrcHandler::begin(port);
}
//////////////////////////////////////////////////////

void GSMUDP::stop()
{
  GSMUDP_ModemUrcHandler::stop();
}

int GSMUDP::endPacket()
{
  return GSMUDP_ModemUrcHandler::endPacket();
}

int GSMUDP::parsePacket()
{
  return GSMUDP_ModemUrcHandler::parsePacket();
}

int GSMUDP::beginPacket(IPAddress ip, uint16_t port)
{
  GSM_LOGDEBUG3(F("GSMUDP::beginPacket: ip = "), ip, F(", port = "), port);
  
  if (_socket < 0)
  {
    return 0;
  }

  _txIp = ip;
  _txHost = NULL;
  _txPort = port;
  _txSize = 0;

  return 1;
}

int GSMUDP::beginPacket(const char *host, uint16_t port)
{
  GSM_LOGDEBUG3(F("GSMUDP::beginPacket: host = "), host, F(", port = "), port);
  
  if (_socket < 0)
  {
    return 0;
  }

  _txIp = (uint32_t)0;
  _txHost = host;
  _txPort = port;
  _txSize = 0;

  return 1;
}

size_t GSMUDP::write(uint8_t b)
{
  return write(&b, sizeof(b));
}

size_t GSMUDP::write(const uint8_t *buffer, size_t size)
{
  if (_socket < 0) 
  {
    return 0;
  }

  size_t spaceAvailable = sizeof(_txBuffer) - _txSize;

  if (size > spaceAvailable) 
  {
    size = spaceAvailable;
  }

  memcpy(&_txBuffer[_txSize], buffer, size);
  _txSize += size;

  return size;
}

int GSMUDP::available()
{
  if (_socket < 0) 
  {
    return 0;
  }

  return (_rxSize - _rxIndex);
}

int GSMUDP::read()
{
  byte b;

  if (read(&b, sizeof(b)) == 1) 
  {
    return b;
  }

  return -1;
}

int GSMUDP::read(unsigned char* buffer, size_t len)
{
  size_t readMax = available();

  if (len > readMax) 
  {
    len = readMax;
  }

  memcpy(buffer, &_rxBuffer[_rxIndex], len);

  _rxIndex += len;

  return len;
}

int GSMUDP::peek()
{
  if (available() > 1) 
  {
    return _rxBuffer[_rxIndex];
  }

  return -1;
}

void GSMUDP::flush()
{
}

IPAddress GSMUDP::remoteIP()
{
  return _rxIp;
}

uint16_t GSMUDP::remotePort()
{
  return _rxPort;
}

#endif    // _GSM_UDP_IMPL_HPP

