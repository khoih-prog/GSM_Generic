/*********************************************************************************************************************************
  defines.h
  
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
 **********************************************************************************************************************************/

#ifndef defines_h
#define defines_h

#if defined(__AVR_ATmega8__) || defined(__AVR_ATmega128__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || \
    defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__) || defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO) || \
    defined(ARDUINO_AVR_MINI) || defined(ARDUINO_AVR_ETHERNET) || defined(ARDUINO_AVR_FIO) || defined(ARDUINO_AVR_BT) || defined(ARDUINO_AVR_LILYPAD) || defined(ARDUINO_AVR_PRO) || \
    defined(ARDUINO_AVR_NG) || defined(ARDUINO_AVR_UNO_WIFI_DEV_ED)
  #error AVR boards not supported. Not enough memory.
#endif

#define DEBUG_GSM_GENERIC_PORT       Serial

// Debug Level from 0 to 5. Level 5 is to print out AT commands and responses
#define _GSM_GENERIC_LOGLEVEL_       5

#define SECRET_PINNUMBER     ""
#define SECRET_GPRS_APN      "GPRS_APN" // replace your GPRS APN
#define SECRET_GPRS_LOGIN    "login"    // replace with your GPRS login
#define SECRET_GPRS_PASSWORD "password" // replace with your GPRS password

//////////////////////////////////////////////

#if defined(ARDUINO_SAMD_MKRGSM1400)

  // For original MKRGSM1400 => GSM_MODEM_UBLOX == true, GSM_MODEM_LARAR2 == false
  // For modified MKRGSM1400 using LARA R2 => GSM_MODEM_UBLOX == false, GSM_MODEM_LARAR2 == true
  #define GSM_MODEM_UBLOX                   false

  #if GSM_MODEM_UBLOX
    #define GSM_MODEM_LARAR2                false
  #else
    #define GSM_MODEM_LARAR2                true
  #endif
    
  #define UBLOX_USING_RESET_PIN             true
  #define UBLOX_USING_LOW_POWER_MODE        true

  #if GSM_MODEM_UBLOX
    #warning Using MKRGSM1400 Configuration with SARA U201
  #elif GSM_MODEM_LARAR2
    #warning Using MKRGSM1400 Configuration with LARA R2xx
  #else
    #error Must select either GSM_MODEM_UBLOX or GSM_MODEM_LARAR2
  #endif
  
#else

  // Optional usage of GSM_RESETN and GSM_DTR. Need to be here only when true. Default is false
  #define UBLOX_USING_RESET_PIN             true
  #define UBLOX_USING_LOW_POWER_MODE        true
  
  // Override the default (and certainly not good) pins and port
  // Only for boards other than ARDUINO_SAMD_MKRGSM1400
  #if (ESP32)
    #define GSM_RESETN  (33u)
    #define GSM_DTR     (34u)
  #elif (ESP8266)
    #define GSM_RESETN  (D3)
    #define GSM_DTR     (D4)
  #else
    #define GSM_RESETN  (10u)
    #define GSM_DTR     (11u)
  #endif

  #if ESP8266
    // Using Software Serial for ESP8266, as Serial1 is TX only
    #define GSM_USING_SOFTWARE_SERIAL     true
  #else
    // Optional Software Serial here for other boards, but not advised if HW Serial available
    #define GSM_USING_SOFTWARE_SERIAL     false
  #endif
   
  #if GSM_USING_SOFTWARE_SERIAL
    #warning Using default SerialGSM = SoftwareSerial
    
    #define D8 (15)
    #define D7 (13)
    
    #include <SoftwareSerial.h>
    
    SoftwareSerial swSerial(D7, D8);    // (D7, D8, false, 256); // (RX, TX, false, 256);
    
    #define SerialGSM   swSerial
  #else
    #warning Using default SerialGSM = HardwareSerial Serial1
    #define SerialGSM   Serial1
  #endif    // GSM_USING_SOFTWARE_SERIAL

  #warning You must connect the Modem correctly and modify the pins / Serial port here
  
  //////////////////////////////////////////////
  
  #define GSM_MODEM_UBLOX             true
  #define GSM_MODEM_SARAR4            false
  #define GSM_MODEM_LARAR2            false
  
  //////////////////////////////////////////////
  // Not supported yet
  #define GSM_MODEM_SIM800            false
  #define GSM_MODEM_SIM808            false
  #define GSM_MODEM_SIM868            false
  #define GSM_MODEM_SIM900            false
  #define GSM_MODEM_SIM5300           false
  #define GSM_MODEM_SIM5320           false
  #define GSM_MODEM_SIM5360           false
  #define GSM_MODEM_SIM7000           false
  #define GSM_MODEM_SIM7100           false
  #define GSM_MODEM_SIM7500           false
  #define GSM_MODEM_SIM7600           false
  #define GSM_MODEM_SIM7800           false
  #define GSM_MODEM_M95               false
  #define GSM_MODEM_BG96              false
  #define GSM_MODEM_A6                false
  #define GSM_MODEM_A7                false
  #define GSM_MODEM_M590              false
  #define GSM_MODEM_MC60              false
  #define GSM_MODEM_MC60E             false
  #define GSM_MODEM_XBEE              false
  #define GSM_MODEM_SEQUANS_MONARCH   false
  //////////////////////////////////////////////

#endif

// libraries
#include <GSM_Generic_Main.h>

#endif    //defines_h
