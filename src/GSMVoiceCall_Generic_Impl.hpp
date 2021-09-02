/*********************************************************************************************************************************
  GSMVoiceCall_Generic_Impl.hpp
  
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

#ifndef _GSM_VOICE_CALL_IMPL_HPP
#define _GSM_VOICE_CALL_IMPL_HPP

//#include "GSMVoiceCall_Generic.h"

GSMVoiceCall::GSMVoiceCall(bool synch)
{
  _synch        = synch;
  _callStatus   = IDLE_CALL;
  
  MODEM.addUrcHandler(this);
}

GSMVoiceCall::~GSMVoiceCall()
{
  MODEM.removeUrcHandler(this);
}

GSM3_voiceCall_st GSMVoiceCall::getvoiceCallStatus()
{
  MODEM.poll();

  return _callStatus;
}

int GSMVoiceCall::ready()
{
  return MODEM.ready();
}

int GSMVoiceCall::voiceCall(const char* to, unsigned long timeout)
{
  GSM_LOGDEBUG1(F("GSMVoiceCall::voiceCall: to = "), to);
  
  if (_synch) 
  {
    while (ready() == 0);
  } 
  else 
  {
    int r = ready();

    if (r == 0) 
    {
      return 0;
    }
  }

  // Dial command ATD
  MODEM.dialNumber(to);

  if (_synch) 
  {
    if (MODEM.waitForResponse(180000) != GSM_RESPONSE_OK) 
    {
      return 0;
    }

    _callStatus = CALLING;
    
    for (unsigned long start = millis(); (timeout == 0) || ((millis() - start) < timeout);) 
    {
      if (getvoiceCallStatus() != CALLING) 
      {
        break;
      }
    }

    if (getvoiceCallStatus() != TALKING) 
    {
      hangCall();
      return 0;
    }

    return 1;
  } 
  else 
  {
    return ready();
  }
}

int GSMVoiceCall::answerCall()
{
  GSM_LOGDEBUG(F("GSMVoiceCall::answerCall"));
  
  if (_synch) 
  {
    while (ready() == 0);
  } 
  else 
  {
    int r = ready();

    if (r == 0) 
    {
      return 0;
    }
  }

  // Answer Incoming Call
  MODEM.answerCall();

  if (_synch) 
  {
    if (MODEM.waitForResponse(20000) == GSM_RESPONSE_OK) 
    {
      return 1;
    }
  }

  return 0;
}

int GSMVoiceCall::hangCall()
{
  GSM_LOGDEBUG(F("GSMVoiceCall::hangCall"));
  
  if (_synch) 
  {
    while (ready() == 0);
  } 
  else 
  {
    int r = ready();

    if (r == 0) 
    {
      return 0;
    }
  }

  // Hangup
  MODEM.hangupCall();

  if (_synch) 
  {
    if (MODEM.waitForResponse(200000) == GSM_RESPONSE_OK) 
    {
      return 1;
    }
  }

  return 0;
}

int GSMVoiceCall::retrieveCallingNumber(char* buffer, int bufsize)
{
  String response;

  if (_synch) 
  {
    while (ready() == 0);
  } 
  else 
  {
    int r = ready();

    if (r == 0) 
    {
      return 0;
    }
  }

  // List current calls +CLCC
  MODEM.retrieveCallingNumber();
  
  if (MODEM.waitForResponse(100, &response) == GSM_RESPONSE_OK) 
  {
    int phoneNumberStartIndex = response.indexOf("\"");
    
    if (phoneNumberStartIndex != -1) 
    {
      int i = phoneNumberStartIndex + 1;

      while (i < (int)response.length() && bufsize > 1) 
      {
        char c = response[i];

        if (c == '"') 
        {
          break;
        }

        *buffer++ = c;
        bufsize--;
        i++;
      }

      *buffer = '\0';
      
      GSM_LOGDEBUG1(F("GSMVoiceCall::retrieveCallingNumber: number = "), buffer);
      
      return 1;
    } 
    else 
    {
      *buffer = '\0';
    }
  }

  return 0;
}

int GSMVoiceCall::enableI2SInput(long sampleRate)
{
  if (disableI2SInput() == 0) 
  {
    return 0;
  }

  int sampleRateNumber;
  
  GSM_LOGDEBUG1(F("GSMVoiceCall::enableI2SInput: sampleRate = "), sampleRate);

  switch (sampleRate) 
  {
    case 8000:
      sampleRateNumber = 0;
      break;

    case 11025:
      sampleRateNumber = 1;
      break;

    case 12000:
      sampleRateNumber = 2;
      break;

    case 16000:
      sampleRateNumber = 3;
      break;

    case 22050:
      sampleRateNumber = 4;
      break;

    case 24000:
      sampleRateNumber = 5;
      break;

    case 32000:
      sampleRateNumber = 6;
      break;

    case 44100:
      sampleRateNumber = 7;
      break;

    case 48000:
      sampleRateNumber = 8;
      break;

    default:
      return 0;
      break;
  }

  // I2S digital interface mode +UI2S
  // mode, port, WA, sample rate, slave
  MODEM.setSampleRate(sampleRateNumber);
  
  if (MODEM.waitForResponse() != GSM_RESPONSE_OK) 
  {
    return 0;
  }

  // enable
  // Audio path mode setting +USPM
  // <main_uplink>, <main_downlink>, <alert_sound>, <headset_indication>, <vmic_ctrl>
  MODEM.enableAudioPathNoHeadset();
  
  if (MODEM.waitForResponse() != GSM_RESPONSE_OK) 
  {
    return 0;
  }
  
  GSM_LOGDEBUG(F("GSMVoiceCall::enableI2SInput: OK"));

  return 1;
}

int GSMVoiceCall::peekDTMF()
{
  MODEM.poll();

  if (_dtmfBuffer.length()) 
  {
    return _dtmfBuffer.charAt(0);
  }

  return -1;
}

int GSMVoiceCall::readDTMF()
{
  MODEM.poll();

  if (_dtmfBuffer.length()) 
  {
    char c = _dtmfBuffer.charAt(0);

    _dtmfBuffer.remove(0, 1);

    return c;
  }

  return -1;
}

int GSMVoiceCall::writeDTMF(char c)
{
  switch (c) 
  {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '#':
    case '*':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
      break;

    default:
      return 0; // invalid
      break;
  }

  // DTMF and tone generation +VTS
  // <DTMF>
  MODEM.setDTMF(c);

  if (MODEM.waitForResponse(420000) != GSM_RESPONSE_OK) 
  {
    return 0;
  }

  return 1;
}

int GSMVoiceCall::disableI2SInput()
{
  // disable
  // Audio path mode setting +USPM
  // <main_uplink>, <main_downlink>, <alert_sound>, <headset_indication>, <vmic_ctrl>
  MODEM.disableAudioPathNoHeadset();

  if (MODEM.waitForResponse() == GSM_RESPONSE_OK) 
  {
    GSM_LOGDEBUG(F("GSMVoiceCall::disableI2SInput: OK"));
    
    return 1;
  }

  return 0;
}

#endif    // _GSM_VOICE_CALL_IMPL_HPP
