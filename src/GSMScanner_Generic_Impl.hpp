/*********************************************************************************************************************************
  GSMScanner_Generic_Impl.hpp
  
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

#ifndef _GSM_SCANNER_HPP
#define _GSM_SCANNER_HPP

//#include "GSMScanner_Generic.h"

GSMScanner::GSMScanner(bool trace)
{
  if (trace) 
  {
    MODEM.debug();
  }
}

GSM3_NetworkStatus_t GSMScanner::begin(unsigned long baud)
{
  MODEM.begin(baud);

  return GSM_IDLE;
}

String GSMScanner::getCurrentCarrier()
{
  String response;

  // Operator selection +COPS => getCurrentCarrier
  MODEM.getCurrentCarrier();
  
  if (MODEM.waitForResponse(180000, &response) == GSM_RESPONSE_OK) 
  {
    GSM_LOGDEBUG1(F("GSMScanner::getCurrentCarrier: response = "), response);
    
    int firstQuoteIndex = response.indexOf('"');
    int lastQuoteIndex = response.lastIndexOf('"');

    if (firstQuoteIndex != -1 && lastQuoteIndex != -1 && firstQuoteIndex != lastQuoteIndex) 
    {
      GSM_LOGDEBUG1(F("GSMScanner::getCurrentCarrier: CurrentCarrier = "), response.substring(firstQuoteIndex + 1, lastQuoteIndex));
      
      return response.substring(firstQuoteIndex + 1, lastQuoteIndex);
    }
  }

  return "";
}

String GSMScanner::getSignalStrength()
{
  String response;

  MODEM.getSignalStrength();
  
  if (MODEM.waitForResponse(100, &response) == GSM_RESPONSE_OK) 
  {
    int firstSpaceIndex = response.indexOf(' ');
    int lastCommaIndex = response.lastIndexOf(',');

    if (firstSpaceIndex != -1 && lastCommaIndex != -1) 
    {
      GSM_LOGDEBUG1(F("GSMScanner::getSignalStrength: SignalStrength = "), response.substring(firstSpaceIndex + 1, lastCommaIndex));
      
      return response.substring(firstSpaceIndex + 1, lastCommaIndex);
    }
  }

  return "";
}

String GSMScanner::readNetworks()
{
  String response;

  // Operator selection +COPS => testCurrentCarrier
  MODEM.testCurrentCarrier();
  
  if (MODEM.waitForResponse(180000, &response) == GSM_RESPONSE_OK) 
  {
    String result;
    unsigned int responseLength = response.length();

    for (unsigned int i = 0; i < responseLength; i++) 
    {
      for (; i < responseLength; i++) 
      {
        if (response[i] == '"') 
        {
          result += "> ";
          break;
        }
      }

      for (i++; i < responseLength; i++) 
      {
        if (response[i] == '"') 
        {
          result += '\n';
          break;
        }

        result += response[i];
      }

      for (i++; i < responseLength; i++) 
      {
        if (response[i] == ')') 
        {
          break;
        }
      }
    }
    
    GSM_LOGDEBUG1(F("GSMScanner::readNetworks: result = "), result);

    return result;
  }

  return "";
}

#endif    // _GSM_SCANNER_HPP
