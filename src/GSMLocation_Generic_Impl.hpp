/*********************************************************************************************************************************
  GSMLocation_Generic_Impl.hpp
  
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

#ifndef _GSM_LOCATION_HPP
#define _GSM_LOCATION_HPP

//#include "GSMLocation_Generic.h"

#define GSM_LOCATION_UPDATE_INTERVAL 1100

GSMLocation::GSMLocation()
{
  _commandSent        = false;
  _locationAvailable  = false;
  _latitude           = 0;
  _longitude          = 0;
  _altitude           = 0;
  _uncertainty        = 0;
  
  MODEM.addUrcHandler(this);
}

GSMLocation::~GSMLocation()
{
  MODEM.removeUrcHandler(this);
}

int GSMLocation::begin()
{
  GSM_LOGDEBUG(F("GSMLocation::begin: send deepScanLocation(AT+ULOCCELL=1)"));
  
  // Configure cellular location sensor. +ULOCCELL
  MODEM.deepScanLocation();

  if (MODEM.waitForResponse() != GSM_RESPONSE_OK) 
  {
    return 0;
  }

  GSM_LOGDEBUG(F("GSMLocation::begin: OK"));
  
  return 1;
}

int GSMLocation::available()
{
  MODEM.poll();

  if (!_commandSent) 
  {
    _commandSent = true;
    _locationAvailable = false;
    
    GSM_LOGDEBUG(F("GSMLocation::available: send getLocation (AT+ULOC=2,2,0,1,1) => Single Shot, Using CellLocate, Std Response, timeout 1s, accuracy 1 meter"));
    
    // Ask for localization information +ULOC. Single Shot, Using CellLocate, Std Response, timeout 1s, accuracy 1 meter
    MODEM.getLocation();
    
    MODEM.waitForResponse();
  }

  if (_locationAvailable) 
  {
    _commandSent = false;
    _locationAvailable = false;

    GSM_LOGDEBUG(F("GSMLocation::available: OK"));
    
    return 1;
  }

  GSM_LOGDEBUG(F("GSMLocation::available: Not available"));
  
  return 0;
}

float GSMLocation::latitude()
{
  GSM_LOGDEBUG1(F("GSMLocation::latitude: "), _latitude);
  
  return _latitude;
}

float GSMLocation::longitude()
{
  GSM_LOGDEBUG1(F("GSMLocation::longitude: "), _longitude);
  
  return _longitude;
}

long GSMLocation::altitude()
{
  GSM_LOGDEBUG1(F("GSMLocation::altitude: "), _altitude);
  
  return _altitude;
}

long GSMLocation::accuracy()
{
  GSM_LOGDEBUG1(F("GSMLocation::accuracy: "), _uncertainty);
  
  return _uncertainty;
}

#endif    // _GSM_LOCATION_HPP
