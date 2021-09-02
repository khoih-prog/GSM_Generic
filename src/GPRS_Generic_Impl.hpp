/*********************************************************************************************************************************
  GPRS_Generic_Impl.hpp
  
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

#ifndef _GPRS_GENERIC_HPP
#define _GPRS_GENERIC_HPP

//#include "GPRS_Generic.h"

GPRS::GPRS()
{
  _timeout    = 0;
  _apn        = NULL;
  _username   = NULL;
  _password   = NULL;
  
  _status = GSM_IDLE;
  
  MODEM.addUrcHandler(this);
}

GPRS::~GPRS()
{
  MODEM.removeUrcHandler(this);
}

GSM3_NetworkStatus_t GPRS::attachGPRS(const char* apn, const char* user_name, const char* password, bool synchronous)
{
  _apn = apn;
  _username = user_name;
  _password = password;

  _state = GPRS_STATE_ATTACH;
  _status = GSM_CONNECTING;

  if (synchronous)
  {
    unsigned long start = millis();

    while (ready() == GSM_RESPONSE_IDLE)
    {
      if (_timeout && !((millis() - start) < _timeout))
      {
        _state = GSM_ERROR;
        break;
      }

      delay(100);
    }
  }
  else
  {
    ready();
  }

  return _status;
}

GSM3_NetworkStatus_t GPRS::detachGPRS(bool synchronous)
{
  _state = GPRS_STATE_DEACTIVATE_IP;

  if (synchronous)
  {
    while (ready() == GSM_RESPONSE_IDLE)
    {
      delay(100);
    }
  }
  else
  {
    ready();
  }

  return _status;
}

void GPRS::setTimeout(unsigned long timeout)
{
  _timeout = timeout;
}

int GPRS::ping(const String &hostname, uint8_t ttl)
{
  return GPRS_ModemUrcHandler::ping(hostname.c_str(), ttl);
}

int GPRS::ping(IPAddress ip, uint8_t ttl)
{
  String host;
  host.reserve(15);

  host += ip[0];
  host += '.';
  host += ip[1];
  host += '.';
  host += ip[2];
  host += '.';
  host += ip[3];

  return ping(host, ttl);
}

GSM3_NetworkStatus_t GPRS::status()
{
  MODEM.poll();

  return _status;
}


///////////////////////////////////////////////

#endif    // _GPRS_GENERIC_HPP
