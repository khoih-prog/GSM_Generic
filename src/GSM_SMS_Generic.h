/*********************************************************************************************************************************
  GSM_SMS_Generic.h
  
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

#ifndef _GSM_SMS_H_INCLUDED
#define _GSM_SMS_H_INCLUDED

#include <Stream.h>

#include "GSM_Generic_Debug.h"


class GSM_SMS : public Stream 
{
  public:
    /** Constructor
        @param synch    Determines sync mode
    */
    GSM_SMS(bool synch = true);

    /** Write a character in SMS message
        @param c      Character
        @return size
    */
    size_t write(uint8_t c);

    /** Begin a SMS to send it
        @param to     Destination
        @return error command if it exists
    */
    int beginSMS(const char* to);

    /** Get last command status
        @return returns 0 if last command is still executing, 1 success, >1 error
    */
    int ready();

    /** End SMS
        @return error command if it exists
    */
    int endSMS();

    /** Check if SMS available and prepare it to be read
        @return number of bytes in a received SMS
    */
    int available();

    /** Read sender number phone
        @param number   Buffer for save number phone
        @param nlength    Buffer length
        @return 1 success, >1 error
    */
    int remoteNumber(char* number, int nlength);

    /** Read one char for SMS buffer (advance circular buffer)
        @return byte
    */
    int read();

    /** Read a byte but do not advance the buffer header (circular buffer)
        @return byte
    */
    int peek();

    /** Delete the SMS from Modem memory and proccess answer
    */
    void flush();

  private:
  
    GSM_SMS_Data  _smsData;
    String        _incomingBuffer;
};

#include "GSM_SMS_Generic_Impl.hpp"

#endif    // _GSM_SMS_H_INCLUDED
