/*********************************************************************************************************************************
  GSMPIN_Generic.h
  
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

#ifndef _GSM_PIN_H_INCLUDED
#define _GSM_PIN_H_INCLUDED

#include <Arduino.h>

#include "GSM_Generic_Debug.h"

#include "GSM_Type_Generic.h"

class GSMPIN 
{
  public:

    /** Constructor */
    GSMPIN();

    /** Check modem response and restart it
        @param baud      baudRate for SerialGSM to communicate to modem 
    */
    void begin(unsigned long baud = 115200);

    /** Check if PIN lock or PUK lock is activated
        @return 0 if PIN lock is off, 1 if PIN lock is on, -1 if PUK lock is on, -2 if error exists
    */
    int isPIN();

    /** Check if PIN code is correct and valid
        @param pin      PIN code
        @return 0 if is correct, -1 if is incorrect
    */
    int checkPIN(String pin);

    /** Check if PUK code is correct and establish new PIN code
        @param puk      PUK code
        @param pin      New PIN code
        @return 0 if successful, otherwise return -1
    */
    int checkPUK(String puk, String pin);

    /** Change PIN code
        @param old      Old PIN code
        @param pin      New PIN code
    */
    void changePIN(String old, String pin);

    /** Change PIN lock status
        @param pin      PIN code
    */
    void switchPIN(String pin);

    /** Check if modem was registered in GSM/GPRS network
        @return 0 if modem was registered, 1 if modem was registered in roaming, -1 if error exists
    */
    int checkReg();

    /** Return if PIN lock is used
        @return true if PIN lock is used, otherwise, return false
    */
    bool getPINUsed();

    /** Set PIN lock status
        @param used     New PIN lock status
    */
    void setPINUsed(bool used);

  private:
    bool _pinUsed;
};

#include "GSMPIN_Generic_Impl.hpp"

#endif    // _GSM_PIN_H_INCLUDED
