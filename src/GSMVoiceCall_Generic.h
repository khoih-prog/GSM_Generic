/*********************************************************************************************************************************
  GSMVoiceCall_Generic.h
  
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

#ifndef _GSM_VOICE_CALL_H_INCLUDED
#define _GSM_VOICE_CALL_H_INCLUDED

#include "GSM_Generic_Debug.h"

#include "GSM_Type_Generic.h"

class GSMVoiceCall : public GSMVoiceCall_ModemUrcHandler 
{
  public:
    /** Service creation
        @param synch    If true, the service calls are synchronois
    */
    GSMVoiceCall(bool synch = true);

    virtual ~GSMVoiceCall();

    /** Voice call status
        @return Status of the voice call, as described in GSM3MobileVoiceProvider.h
              { IDLE_CALL, CALLING, RECEIVINGCALL, TALKING};
    */
    GSM3_voiceCall_st getvoiceCallStatus();

    /** Get last command status
        @return Returns 0 if last command is still executing, 1 success, >1 error
    */
    int ready();

    /** Place a voice call. If asynchronous, returns while ringing. If synchronous
        returns if the call is stablished or cancelled.
        @param to         Receiver number. Country extension can be used or not.
                          Char buffer should not be released or used until command is over
        @param timeout    In millisecods. Time ringing before closing the call.
                          Only used in synchronous mode.
                          If zero, ring undefinitely
        @return In asynchronous mode returns 0 if last command is still executing, 1 success, >1 error
                In synchronous mode returns 1 if the call is placed, 0 if not.
    */
    int voiceCall(const char* to, unsigned long timeout = 30000);

    /** Accept an incoming voice call
        @return In asynchronous mode returns 0 if last command is still executing, 1 success, >1 error
                In synchronous mode returns 1 if the call is answered, 0 if not.
    */
    int answerCall();

    /** Hang a stablished call or an incoming ring
        @return In asynchronous mode returns 0 if last command is still executing, 1 success, >1 error
                In synchronous mode returns 1 if the call is answered, 0 if not.
    */
    int hangCall();

    /** Retrieve the calling number, put it in buffer
        @param buffer     pointer to the buffer memory
        @param bufsize    size of available memory area, at least should be 10 characters
        @return In asynchronous mode returns 0 if last command is still executing, 1 success, >1 error
                In synchronous mode returns 1 if the number is correcty taken 0 if not
    */
    int retrieveCallingNumber(char* buffer, int bufsize);

    int peekDTMF();
    int readDTMF();
    int writeDTMF(char c);

    int enableI2SInput(long sampleRate);
    int disableI2SInput();
   
  private:
   
};


#include "GSMVoiceCall_Generic_Impl.hpp"

#endif    // _GSM_VOICE_CALL_H_INCLUDED
