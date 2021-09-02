/*********************************************************************************************************************************
  GSM_Generic_Impl.hpp
  
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

#ifndef _GSM_HPP
#define _GSM_HPP

#define _XOPEN_SOURCE

#include <time.h>

extern "C" 
{
  char *strptime(const char *__restrict, const char *__restrict, struct tm *__restrict);
}

GSM::GSM(bool debug)
{
  _gsmData._state       = GSM_ERROR;
  _gsmData._readyState  = 0;
  _gsmData._pin         = NULL;
  _gsmData._timeout     = 0;
    
  if (debug) 
  {
    MODEM.debug();
  }
}

GSM3_NetworkStatus_t GSM::begin(const char* pin, bool restart, bool synchronous)
{
  if (MODEM.begin(restart) == GSM_MODEM_START_ERROR) 
  {
    _gsmData._state = GSM_ERROR;
  } 
  else 
  {
    _gsmData._pin = pin;
    _gsmData._state = GSM_IDLE;
    _gsmData._readyState = READY_STATE_CHECK_SIM;

    if (synchronous) 
    {
      unsigned long start = millis();

      while (ready() == GSM_RESPONSE_IDLE) 
      {
        if (_gsmData._timeout && !((millis() - start) < _gsmData._timeout)) 
        {
          _gsmData._state = GSM_ERROR;
          break;
        }

        delay(100);
      }
    } 
    else 
    {
      return (GSM3_NetworkStatus_t) 0;
    }
  }
  
  GSM_LOGDEBUG1(F("GSM::begin: _gsmData._state = "), (_gsmData._state == GSM_ERROR) ? "GSM_ERROR" : "GSM_IDLE" );

  return _gsmData._state;
}

GSM3_NetworkStatus_t GSM::begin(unsigned long baud, const char* pin, bool restart, bool synchronous)
{
  if (MODEM.begin(baud, restart) == GSM_MODEM_START_ERROR) 
  {
    _gsmData._state = GSM_ERROR;
  } 
  else 
  {
    _gsmData._pin         = pin;
    _gsmData._state       = GSM_IDLE;
    _gsmData._readyState  = READY_STATE_CHECK_SIM;

    if (synchronous) 
    {
      unsigned long start = millis();

      while (ready() == GSM_RESPONSE_IDLE) 
      {
        if (_gsmData._timeout && !((millis() - start) < _gsmData._timeout)) 
        {
          _gsmData._state = GSM_ERROR;
          break;
        }

        delay(100);
      }
    } 
    else 
    {
      return (GSM3_NetworkStatus_t) 0;
    }
  }
  
  GSM_LOGDEBUG1(F("GSM::begin: _gsmData._state = "), (_gsmData._state == GSM_ERROR) ? "GSM_ERROR" : "GSM_IDLE" );

  return _gsmData._state;
}

int GSM::isAccessAlive()
{
  return MODEM.isAccessAlive();
}

bool GSM::shutdown()
{
  return MODEM.shutdown(_gsmData._state);
}

bool GSM::secureShutdown()
{
  MODEM.end();
  _gsmData._state = GSM_OFF;
  
  return true;
}

int GSM::ready()
{
  return MODEM.GSM_ready(_gsmData, _response);
}

void GSM::setTimeout(unsigned long timeout)
{
  _gsmData._timeout = timeout;
}

unsigned long GSM::getTime()
{
  return MODEM.getTime();
}

unsigned long GSM::getLocalTime()
{
  return MODEM.getLocalTime();
}

int GSM::lowPowerMode()
{
  return MODEM.lowPowerMode();
}

int GSM::noLowPowerMode()
{
  return MODEM.noLowPowerMode();
}

GSM3_NetworkStatus_t GSM::status()
{
  return _gsmData._state;
}

#endif    // _GSM_HPP
