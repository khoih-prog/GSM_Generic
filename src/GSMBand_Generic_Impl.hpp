/*********************************************************************************************************************************
  GSMBand_Generic_Impl.hpp
  
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
 
  Version: 1.3.0
  
  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.2.4    K Hoang     11/03/2021 Initial public release to add support to many boards / modules besides MKRGSM 1400 / SARA U201
  1.3.0    K Hoang     31/03/2021 Add ThingStream MQTTS support. Fix SMS receive bug.
 **********************************************************************************************************************************/

#pragma once

#ifndef _GSM_BAND_HPP
#define _GSM_BAND_HPP

//#include "GSMBand_Generic.h"

GSMBand::GSMBand(bool trace)
{
  if (trace) 
  {
    MODEM.debug();
  }
}

GSM3_NetworkStatus_t GSMBand::begin(unsigned long baud)
{
  return (GSM3_NetworkStatus_t) MODEM.begin(baud);
}

bool GSMBand::setRAT(const char* act) 
{
  return MODEM.setRAT(act);
}

String GSMBand::getBand()
{
  return MODEM.getBand();
}

bool GSMBand::setBand(String band)
{
  return MODEM.setBand(band);
}

#endif    // _GSM_BAND_HPP
