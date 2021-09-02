/*********************************************************************************************************************************
  GSM_Generic.h
  
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

#ifndef _GSM_H_INCLUDED
#define _GSM_H_INCLUDED

#include <Arduino.h>

#include "GSM_Generic_Debug.h"

#include "GSM_Type_Generic.h"

class GSM 
{
  public:
    /** Constructor
        @param debug    Determines debug mode
    */
    GSM(bool debug = false);

    /** Start the GSM/GPRS modem, attaching to the GSM network
        @param pin         SIM PIN number (4 digits in a string, example: "1234"). If
                           NULL the SIM has no configured PIN.
        @param restart     Restart the modem. Default is TRUE. The modem receives
                           a signal through the Ctrl/D7 pin. If it is shut down, it will
                           start-up. If it is running, it will restart. Takes up to 10
                           seconds
        @param synchronous If TRUE the call only returns after the Start is complete
                           or fails. If FALSE the call will return inmediately. You have
                           to call repeatedly ready() until you get a result. Default is TRUE.
        @return If synchronous, GSM3_NetworkStatus_t. If asynchronous, returns 0.
    */
    GSM3_NetworkStatus_t begin(const char* pin = 0, bool restart = true, bool synchronous = true);
    GSM3_NetworkStatus_t begin(unsigned long baud = 115200, const char* pin = 0, bool restart = true, bool synchronous = true);

    /** Check network access status
        @return 1 if Alive, 0 if down
    */
    int isAccessAlive();

    /** Shutdown the modem (power off really)
        @return true if successful
    */
    bool shutdown();

    /** Secure shutdown the modem (power off really)
        @return always true
    */
    bool secureShutdown();

    /** Get last command status
        @return returns 0 if last command is still executing, 1 success, >1 error
    */
    int ready();

    void setTimeout(unsigned long timeout);

    unsigned long getTime();
    unsigned long getLocalTime();

    int lowPowerMode();
    int noLowPowerMode();

    GSM3_NetworkStatus_t status();

  private:

    GSM_Data  _gsmData;
    String    _response;
};

#include "GSM_Generic_Impl.hpp"

#endif    // _GSM_H_INCLUDED
