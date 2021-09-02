/*********************************************************************************************************************************
  GSMPIN_Generic_Impl.hpp
  
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

#ifndef _GSM_PIN_HPP
#define _GSM_PIN_HPP

//#include "GSMPIN_Generic.h"

GSMPIN::GSMPIN() :
  _pinUsed(false)
{
}

void GSMPIN::begin(unsigned long baud)
{
  MODEM.begin(baud);
}

int GSMPIN::isPIN()
{
  String response;

  for (unsigned long start = millis(); (millis() - start) < 1000;) 
  {
    // Enter PIN +CPIN. Read PIN
    MODEM.readPIN();

    if (MODEM.waitForResponse(10000, &response) == GSM_RESPONSE_OK) 
    {
      if (response.startsWith("+CPIN: ")) 
      {
        if (response.endsWith("READY")) 
        {
          GSM_LOGDEBUG(F("GSMPIN::isPIN: READY"));
          
          return SIM_PIN_UNLOCKED;
        } 
        else if (response.endsWith("SIM PIN")) 
        {
          GSM_LOGDEBUG(F("GSMPIN::isPIN: SIM PIN"));
          
          return SIM_PIN_NEED_PIN;
        } 
        else if (response.endsWith("SIM PUK")) 
        {
          GSM_LOGDEBUG(F("GSMPIN::isPIN: SIM PUK"));
          
          return SIM_PIN_NEED_PUK;
        } 
        else 
        {
          GSM_LOGDEBUG(F("GSMPIN::isPIN: Error"));
          
          return SIM_PIN_ERROR;
        }
      }
    }

    delay(100);
  }
  
  GSM_LOGDEBUG(F("GSMPIN::isPIN: Error"));

  return SIM_PIN_ERROR;
}

int GSMPIN::checkPIN(String pin)
{
  GSM_LOGDEBUG1(F("GSMPIN::checkPIN: pin = "), pin);
  
  // Enter PIN +CPIN. Check PIN
  MODEM.checkPIN(pin);
  
  if (MODEM.waitForResponse(10000) == GSM_RESPONSE_OK) 
  {
    GSM_LOGDEBUG(F("GSMPIN::checkPIN: OK"));
    
    return SIM_PIN_UNLOCKED;
  }

  GSM_LOGDEBUG(F("GSMPIN::checkPIN: Error"));
  
  return SIM_PIN_ERROR;
}


int GSMPIN::checkPUK(String puk, String pin)
{
  GSM_LOGDEBUG3(F("GSMPIN::checkPUK: puk = "), puk, F(", pin = "), pin);
   
  // Enter PIN +CPIN. Check PUK 
  MODEM.checkPUK(puk, pin);
  
  if (MODEM.waitForResponse(10000) == GSM_RESPONSE_OK) 
  {
    GSM_LOGDEBUG(F("GSMPIN::checkPUK: OK"));
    
    return SIM_PIN_UNLOCKED;
  }
  
  GSM_LOGDEBUG(F("GSMPIN::checkPUK: Error"));

  return SIM_PIN_ERROR;
}

void GSMPIN::changePIN(String old, String pin)
{
  GSM_LOGDEBUG3(F("GSMPIN::changePIN: old = "), old, F(", new pin = "), pin);
  
  // Enter PIN +CPIN. Change PIN
  MODEM.changePIN(old, pin);
  
  if (MODEM.waitForResponse(10000) == GSM_RESPONSE_OK) 
  {
    GSM_LOGDEBUG(F("GSMPIN::changePIN: OK"));
  } 
  else 
  {
    GSM_LOGDEBUG(F("GSMPIN::changePIN: Error"));
  }
}

void GSMPIN::switchPIN(String pin)
{
  MODEM.switchPIN(pin, _pinUsed);
}

int GSMPIN::checkReg()
{
  return MODEM.checkReg(); 
}

bool GSMPIN::getPINUsed()
{
  GSM_LOGDEBUG1(F("GSMPIN::getPINUsed: _pinUsed = "), _pinUsed);
  
  return _pinUsed;
}

void GSMPIN::setPINUsed(bool used)
{ 
  _pinUsed = used;
  
  GSM_LOGDEBUG1(F("GSMPIN::getPINUsed: _pinUsed = "), _pinUsed);
}

#endif    // _GSM_PIN_HPP

