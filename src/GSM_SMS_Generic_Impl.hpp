/*********************************************************************************************************************************
  GSM_SMS_Generic_Impl.hpp
  
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

#ifndef _GSM_SMS_HPP
#define _GSM_SMS_HPP

//#include "GSM_SMS_Generic.h"

GSM_SMS::GSM_SMS(bool synch)
{
  _smsData.synch         = synch;
  _smsData.state         = SMS_STATE_IDLE;
  _smsData.smsTxActive   = false;
}

size_t GSM_SMS::write(uint8_t c)
{
  if (_smsData.smsTxActive)
  {
    return MODEM.write(c);
  }

  return 0;
}

int GSM_SMS::beginSMS(const char* to)
{
  // Send message +CMGS => send SMS to
  MODEM.sendSMS_To(to);

  if (MODEM.waitForResponse(100) == GSM_RESPONSE_ERROR /*2*/)
  {
    _smsData.smsTxActive = false;

    return (_smsData.synch) ? 0 : 2;
  }

  _smsData.smsTxActive = true;

  return 1;
}

int GSM_SMS::ready()
{
  return MODEM.readySMS(_smsData, _incomingBuffer);
}

int GSM_SMS::endSMS()
{
  int r;

  if (_smsData.smsTxActive)
  {
    MODEM.write(26);

    if (_smsData.synch) 
    {
      while ((r = MODEM.ready()) == 0) 
      {
        delay(100);
      }
    } 
    else 
    {
      r = MODEM.ready();
    }

    return r;
  } 
  else 
  {
    return (_smsData.synch ? 0 : 2);
  }
}


int GSM_SMS::available()
{
  return MODEM.availableSMS(_smsData, _incomingBuffer);
}

int GSM_SMS::remoteNumber(char* number, int nlength)
{
#define PHONE_NUMBER_START_SEARCH_PATTERN "\"REC UNREAD\",\""

  int phoneNumberStartIndex = _incomingBuffer.indexOf(PHONE_NUMBER_START_SEARCH_PATTERN);

  if (phoneNumberStartIndex != -1) 
  {
    int i = phoneNumberStartIndex + sizeof(PHONE_NUMBER_START_SEARCH_PATTERN) - 1;

    while (i < (int)_incomingBuffer.length() && nlength > 1) 
    {
      char c = _incomingBuffer[i];

      if (c == '"') 
      {
        break;
      }

      *number++ = c;
      nlength--;
      i++;
    }

    *number = '\0';
    
    GSM_LOGDEBUG1(F("GSM_SMS::remoteNumber: number = "), number);
    
    return 1;
  } 
  else 
  {    
    *number = '\0';
  }

  return 2;
}

int GSM_SMS::read()
{
  int bufferLength = _incomingBuffer.length();

  if (_smsData.smsDataIndex < bufferLength && _smsData.smsDataIndex <= _smsData.smsDataEndIndex) 
  {
    return _incomingBuffer[_smsData.smsDataIndex++];
  }

  return -1;
}

int GSM_SMS::peek()
{
  if (_smsData.smsDataIndex < (int)_incomingBuffer.length() && _smsData.smsDataIndex <= _smsData.smsDataEndIndex) 
  {
    return _incomingBuffer[_smsData.smsDataIndex];
  }

  return -1;
}

void GSM_SMS::flush()
{
  int smsIndexStart = _incomingBuffer.indexOf(' ');
  int smsIndexEnd = _incomingBuffer.indexOf(',');

  if (smsIndexStart != -1 && smsIndexEnd != -1) 
  {
    while (MODEM.ready() == 0);

    // Delete SMS message +CMGD => delete SMS at index
    MODEM.deleteSMSAtIndex(_incomingBuffer.substring(smsIndexStart + 1, smsIndexEnd).c_str());

    if (_smsData.synch) 
    {
      MODEM.waitForResponse(55000);
    }
  }
}

#endif    // _GSM_SMS_HPP

