/*********************************************************************************************************************************
  GSM_Generic_Debug.h
  
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


#ifndef _GSM_GENERIC_DEBUG_H_INCLUDED
#define _GSM_GENERIC_DEBUG_H_INCLUDED

#ifdef DEBUG_GSM_GENERIC_PORT
  #define DBG_PORT_GSM       DEBUG_GSM_GENERIC_PORT
#else
  #define DBG_PORT_GSM       Serial
#endif

// Change _GSM_GENERIC_LOGLEVEL_ to set tracing and logging verbosity
// 0: DISABLED: no logging
// 1: ERROR: errors
// 2: WARN: errors and warnings
// 3: INFO: errors, warnings and informational (default)
// 4: DEBUG: errors, warnings, informational and debug
// 5: ATDEBUG: errors, warnings, informational and debug including AT-commands

#ifndef _GSM_GENERIC_LOGLEVEL_
  #define _GSM_GENERIC_LOGLEVEL_       0
#endif

const char GSM_MARK[] = "[GSM] ";

#define GSM_PRINT_MARK   DBG_PORT_GSM.print(GSM_MARK)

#define GSM_PRINT        DBG_PORT_GSM.print
#define GSM_PRINTLN      DBG_PORT_GSM.println


///////////////////////////////////////////

#define GSM_LOGERROR0(x)              if(_GSM_GENERIC_LOGLEVEL_>0) { GSM_PRINT(x); }
#define GSM_LOGERROR(x)               if(_GSM_GENERIC_LOGLEVEL_>0) { GSM_PRINT_MARK; GSM_PRINTLN(x); }
#define GSM_LOGERROR1(x,y)            if(_GSM_GENERIC_LOGLEVEL_>0) { GSM_PRINT_MARK; GSM_PRINT(x); GSM_PRINTLN(y); }
#define GSM_LOGERROR2(x,y,z)          if(_GSM_GENERIC_LOGLEVEL_>0) { GSM_PRINT_MARK; GSM_PRINT(x); GSM_PRINT(y); GSM_PRINTLN(z); }
#define GSM_LOGERROR3(x,y,z,w)        if(_GSM_GENERIC_LOGLEVEL_>0) { GSM_PRINT_MARK; GSM_PRINT(x); GSM_PRINT(y); GSM_PRINT(z); GSM_PRINTLN(w); }
#define GSM_LOGERROR5(x,y,z,w,xx,yy)  if(_GSM_GENERIC_LOGLEVEL_>0) { GSM_PRINT_MARK; GSM_PRINT(x); GSM_PRINT(y); GSM_PRINT(z); GSM_PRINT(w); GSM_PRINT(xx); GSM_PRINTLN(yy); }

///////////////////////////////////////////

#define GSM_LOGWARN0(x)             if(_GSM_GENERIC_LOGLEVEL_>1) { GSM_PRINT(x); }
#define GSM_LOGWARN(x)              if(_GSM_GENERIC_LOGLEVEL_>1) { GSM_PRINT_MARK; GSM_PRINTLN(x); }
#define GSM_LOGWARN1(x,y)           if(_GSM_GENERIC_LOGLEVEL_>1) { GSM_PRINT_MARK; GSM_PRINT(x); GSM_PRINTLN(y); }
#define GSM_LOGWARN2(x,y,z)         if(_GSM_GENERIC_LOGLEVEL_>1) { GSM_PRINT_MARK; GSM_PRINT(x); GSM_PRINT(y); GSM_PRINTLN(z); }
#define GSM_LOGWARN3(x,y,z,w)       if(_GSM_GENERIC_LOGLEVEL_>1) { GSM_PRINT_MARK; GSM_PRINT(x); GSM_PRINT(y); GSM_PRINT(z); GSM_PRINTLN(w); }
#define GSM_LOGWARN5(x,y,z,w,xx,yy) if(_GSM_GENERIC_LOGLEVEL_>1) { GSM_PRINT_MARK; GSM_PRINT(x); GSM_PRINT(y); GSM_PRINT(z); GSM_PRINT(w); GSM_PRINT(xx); GSM_PRINTLN(yy); }

///////////////////////////////////////////

#define GSM_LOGINFO0(x)             if(_GSM_GENERIC_LOGLEVEL_>2) { GSM_PRINT(x); }
#define GSM_LOGINFO(x)              if(_GSM_GENERIC_LOGLEVEL_>2) { GSM_PRINT_MARK; GSM_PRINTLN(x); }
#define GSM_LOGINFO1(x,y)           if(_GSM_GENERIC_LOGLEVEL_>2) { GSM_PRINT_MARK; GSM_PRINT(x); GSM_PRINTLN(y); }
#define GSM_LOGINFO2(x,y,z)         if(_GSM_GENERIC_LOGLEVEL_>3) { GSM_PRINT_MARK; GSM_PRINT(x); GSM_PRINT(y); GSM_PRINTLN(z); }
#define GSM_LOGINFO3(x,y,z,w)       if(_GSM_GENERIC_LOGLEVEL_>3) { GSM_PRINT_MARK; GSM_PRINT(x); GSM_PRINT(y); GSM_PRINT(z); GSM_PRINTLN(w); }
#define GSM_LOGINFO5(x,y,z,w,xx,yy) if(_GSM_GENERIC_LOGLEVEL_>2) { GSM_PRINT_MARK; GSM_PRINT(x); GSM_PRINT(y); GSM_PRINT(z); GSM_PRINT(w); GSM_PRINT(xx); GSM_PRINTLN(yy); }

///////////////////////////////////////////

#define GSM_LOGDEBUG0(x)              if(_GSM_GENERIC_LOGLEVEL_>3) { GSM_PRINT(x); }
#define GSM_LOGDEBUG(x)               if(_GSM_GENERIC_LOGLEVEL_>3) { GSM_PRINT_MARK; GSM_PRINTLN(x); }
#define GSM_LOGDEBUG1(x,y)            if(_GSM_GENERIC_LOGLEVEL_>3) { GSM_PRINT_MARK; GSM_PRINT(x); GSM_PRINTLN(y); }
#define GSM_LOGDEBUG2(x,y,z)          if(_GSM_GENERIC_LOGLEVEL_>3) { GSM_PRINT_MARK; GSM_PRINT(x); GSM_PRINT(y); GSM_PRINTLN(z); }
#define GSM_LOGDEBUG3(x,y,z,w)        if(_GSM_GENERIC_LOGLEVEL_>3) { GSM_PRINT_MARK; GSM_PRINT(x); GSM_PRINT(y); GSM_PRINT(z); GSM_PRINTLN(w); }
#define GSM_LOGDEBUG5(x,y,z,w,xx,yy)  if(_GSM_GENERIC_LOGLEVEL_>3) { GSM_PRINT_MARK; GSM_PRINT(x); GSM_PRINT(y); GSM_PRINT(z); GSM_PRINT(w); GSM_PRINT(xx); GSM_PRINTLN(yy); }

///////////////////////////////////////////

#define GSM_LOGATDEBUG0(x)              if(_GSM_GENERIC_LOGLEVEL_>4) { GSM_PRINT(x); }
#define GSM_LOGATDEBUG(x)               if(_GSM_GENERIC_LOGLEVEL_>4) { GSM_PRINT_MARK; GSM_PRINTLN(x); }
#define GSM_LOGATDEBUG1(x,y)            if(_GSM_GENERIC_LOGLEVEL_>4) { GSM_PRINT_MARK; GSM_PRINT(x); GSM_PRINTLN(y); }
#define GSM_LOGATDEBUG2(x,y,z)          if(_GSM_GENERIC_LOGLEVEL_>4) { GSM_PRINT_MARK; GSM_PRINT(x); GSM_PRINT(y); GSM_PRINTLN(z); }
#define GSM_LOGATDEBUG3(x,y,z,w)        if(_GSM_GENERIC_LOGLEVEL_>4) { GSM_PRINT_MARK; GSM_PRINT(x); GSM_PRINT(y); GSM_PRINT(z); GSM_PRINTLN(w); }
#define GSM_LOGATDEBUG5(x,y,z,w,xx,yy)  if(_GSM_GENERIC_LOGLEVEL_>4) { GSM_PRINT_MARK; GSM_PRINT(x); GSM_PRINT(y); GSM_PRINT(z); GSM_PRINT(w); GSM_PRINT(xx); GSM_PRINTLN(yy); }

///////////////////////////////////////////

#endif    // _GSM_GENERIC_DEBUG_H_INCLUDED
