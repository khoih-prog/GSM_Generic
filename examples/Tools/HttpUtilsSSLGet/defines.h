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

// GSM settings
#define SECRET_PINNUMBER     ""
#define SECRET_GPRS_APN      "GPRS_APN" // replace your GPRS APN
#define SECRET_GPRS_LOGIN    "login"    // replace with your GPRS login
#define SECRET_GPRS_PASSWORD "password" // replace with your GPRS password

////////////////////////////////////////////////////////////

#if ( defined(ARDUINO_NUCLEO_F767ZI) || defined(ARDUINO_NUCLEO_L053R8) )
  HardwareSerial Serial1(D0, D1);   // (PA3, PA2) for ARDUINO_NUCLEO_L053R8
#endif

//////////////////////////////////////////////

#if !defined(ARDUINO_SAMD_MKRGSM1400)
  // Override the default (and certainly not good) pins and port
  // Only for boards other than ARDUINO_SAMD_MKRGSM1400
  #define GSM_RESETN  (10u)
  #define GSM_DTR     (11u)

  #if !ESP8266
    #define SerialGSM   Serial1
  #else
    #warning Using default SerialGSM = Serial => can not use Serial for Debug Terminal

    #define SerialGSM   Serial
  #endif

  #warning You must connect the Modem correctly and modify the pins / Serial port here
#endif

//////////////////////////////////////////////

// Optional usage of GSM_RESETN and GSM_DTR. Need to be here only when true. Default is false

//#define UBLOX_USING_RESET_PIN             true
//#define UBLOX_USING_LOW_POWER_MODE        true

//////////////////////////////////////////////

#define GSM_MODEM_UBLOX             true
#define GSM_MODEM_SARAR4            false

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

// libraries
#include <GSM_Generic_Main.h>

////////////////////////////////////////////////////////////

// Replace with your client.crt certificate in DER format
const uint8_t SECRET_CERT[] = 
{
  /*0x30, 0x82, 0x03, 0x41, 0x30, 0x82, 0x02, 0x29, 0x02, 0x09, 0x00, 0xc2,
  0x68, 0x43, 0x0c, 0x54, 0xae, 0xfe, 0x3a, 0x30, 0x0d, 0x06, 0x09, 0x2a,
  0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x30, 0x62,
  0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x49,
  0x54, 0x31, 0x0e, 0x30, 0x0c, 0x06, 0x03, 0x55, 0x04, 0x08, 0x0c, 0x05,
  0x49, 0x74, 0x61, 0x6c, 0x79, 0x31, 0x0e, 0x30, 0x0c, 0x06, 0x03, 0x55,
  0x04, 0x07, 0x0c, 0x05, 0x54, 0x75, 0x72, 0x69, 0x6e, 0x31, 0x10, 0x30,
  0x0e, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x07, 0x41, 0x72, 0x64, 0x75,
  0x69, 0x6e, 0x6f, 0x31, 0x0c, 0x30, 0x0a, 0x06, 0x03, 0x55, 0x04, 0x0b,
  0x0c, 0x03, 0x4d, 0x6e, 0x78, 0x31, 0x13, 0x30, 0x11, 0x06, 0x03, 0x55,
  0x04, 0x03, 0x0c, 0x0a, 0x63, 0x6c, 0x65, 0x61, 0x6e, 0x69, 0x75, 0x6d,
  0x43, 0x41, 0x30, 0x1e, 0x17, 0x0d, 0x32, 0x30, 0x30, 0x36, 0x31, 0x36,
  0x31, 0x35, 0x32, 0x39, 0x30, 0x31, 0x5a, 0x17, 0x0d, 0x32, 0x31, 0x30,
  0x36, 0x31, 0x36, 0x31, 0x35, 0x32, 0x39, 0x30, 0x31, 0x5a, 0x30, 0x63,
  0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x49,
  0x54, 0x31, 0x11, 0x30, 0x0f, 0x06, 0x03, 0x55, 0x04, 0x08, 0x0c, 0x08,
  0x50, 0x69, 0x65, 0x64, 0x6d, 0x6f, 0x6e, 0x74, 0x31, 0x0e, 0x30, 0x0c,
  0x06, 0x03, 0x55, 0x04, 0x07, 0x0c, 0x05, 0x54, 0x75, 0x72, 0x69, 0x6e,
  0x31, 0x0c, 0x30, 0x0a, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x03, 0x4d,
  0x6e, 0x78, 0x31, 0x10, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x04, 0x0b, 0x0c,
  0x07, 0x41, 0x72, 0x64, 0x75, 0x69, 0x6e, 0x6f, 0x31, 0x11, 0x30, 0x0f,
  0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x08, 0x4d, 0x4b, 0x52, 0x47, 0x53,
  0x4d, 0x30, 0x31, 0x30, 0x82, 0x01, 0x22, 0x30, 0x0d, 0x06, 0x09, 0x2a,
  0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x82,
  0x01, 0x0f, 0x00, 0x30, 0x82, 0x01, 0x0a, 0x02, 0x82, 0x01, 0x01, 0x00,
  0xb6, 0xb7, 0xa2, 0x73, 0x9e, 0x8b, 0xcc, 0x7f, 0x55, 0xb3, 0xb4, 0xad,
  0xfe, 0x6a, 0xbc, 0xa7, 0xe5, 0xec, 0x02, 0xd0, 0x4b, 0x25, 0xa4, 0xd3,
  0xc4, 0x33, 0x37, 0xa7, 0x76, 0xdf, 0xa3, 0x9f, 0xdd, 0x42, 0x3e, 0xb5,
  0xe2, 0xbc, 0x89, 0xe4, 0x18, 0x02, 0xb6, 0x62, 0xe4, 0x3b, 0xa6, 0x74,
  0x76, 0xf8, 0x73, 0xe7, 0x80, 0xea, 0x5d, 0x93, 0x9d, 0x61, 0x2a, 0x24,
  0x06, 0x4c, 0x6b, 0x68, 0xea, 0x63, 0x1d, 0x3a, 0xce, 0x8b, 0x31, 0x05,
  0x72, 0xe9, 0xdc, 0xce, 0x05, 0x8c, 0x7a, 0xc1, 0xf5, 0xd6, 0xe1, 0xd0,
  0x6d, 0x73, 0x4e, 0xc1, 0x8a, 0x16, 0x49, 0x4d, 0xac, 0x21, 0xfb, 0x2d,
  0x12, 0xdd, 0x1e, 0xef, 0x9b, 0xb3, 0x6d, 0x20, 0x69, 0xfb, 0xaa, 0x3c,
  0x2c, 0x8f, 0x0d, 0x7c, 0x09, 0x1c, 0x1a, 0xa5, 0x23, 0x83, 0x98, 0x10,
  0xf4, 0x0c, 0x78, 0xa4, 0xa0, 0x4b, 0x1a, 0x71, 0x52, 0x4b, 0xb3, 0x8b,
  0x4c, 0x29, 0xfe, 0x6f, 0x24, 0x88, 0x7e, 0xd6, 0xf0, 0x6a, 0x2c, 0x3c,
  0x6c, 0xad, 0x54, 0x0c, 0xb6, 0x7b, 0xa4, 0x76, 0x38, 0xbf, 0x35, 0xc6,
  0x66, 0xde, 0x6a, 0x92, 0x43, 0x26, 0x31, 0xae, 0x76, 0xb5, 0xf6, 0x15,
  0x87, 0x02, 0x37, 0x7e, 0x7b, 0x92, 0xda, 0x3f, 0x45, 0x1e, 0x89, 0xb7,
  0x26, 0xb3, 0x8d, 0x3c, 0xf6, 0x49, 0x25, 0xe9, 0x1e, 0x52, 0x6f, 0x40,
  0x2e, 0xb6, 0x7d, 0x5c, 0x2a, 0x8a, 0x9c, 0x7d, 0x2c, 0x09, 0xce, 0xde,
  0x73, 0x5d, 0x3b, 0x7d, 0x4f, 0x3f, 0x4b, 0xdc, 0xeb, 0x8b, 0x93, 0xd1,
  0xc2, 0xf3, 0x29, 0x08, 0x98, 0x06, 0x8c, 0x46, 0x38, 0xe1, 0xd5, 0x30,
  0xc1, 0xf0, 0x3c, 0x74, 0x58, 0xf0, 0x20, 0x8b, 0xea, 0x30, 0x43, 0xd8,
  0x3d, 0x19, 0xc4, 0xd1, 0x04, 0xc8, 0xe5, 0x76, 0x31, 0x36, 0x4a, 0xae,
  ..    ..    ..    ..    ..    ..   ..    ..    ..    ..    ..    ..
  0xd3, 0xb2, 0x75, 0x2b, 0x85, 0x71, 0x02, 0x9a, 0x51, 0x38, 0x08, 0x61,
  0xf8, 0x8b, 0x8b, 0x36, 0xf5, 0x10, 0x6e, 0xd9, 0xda, 0xf5, 0x3a, 0xf1,
  0xf1, 0x6f, 0xc4, 0xd7, 0x52, 0x6f, 0x22, 0x38, 0x8a, 0xff, 0x75, 0x82,
  0x93, 0x94, 0xc9, 0x45, 0xcf, 0xa3, 0x23, 0x04, 0x6f, 0x3a, 0x58, 0xe1,
  0xca, 0xfa, 0xa8, 0x1f, 0xd3, 0x87, 0x6e, 0xcb, 0xe3*/
};

// Replace with your client.key certificate in DER format
uint8_t SECRET_KEY[] = {
  /*0x30, 0x82, 0x04, 0xa4, 0x02, 0x01, 0x00, 0x02, 0x82, 0x01, 0x01, 0x00,
  0xb6, 0xb7, 0xa2, 0x73, 0x9e, 0x8b, 0xcc, 0x7f, 0x55, 0xb3, 0xb4, 0xad,
  0xfe, 0x6a, 0xbc, 0xa7, 0xe5, 0xec, 0x02, 0xd0, 0x4b, 0x25, 0xa4, 0xd3,
  0xc4, 0x33, 0x37, 0xa7, 0x76, 0xdf, 0xa3, 0x9f, 0xdd, 0x42, 0x3e, 0xb5,
  0xe2, 0xbc, 0x89, 0xe4, 0x18, 0x02, 0xb6, 0x62, 0xe4, 0x3b, 0xa6, 0x74,
  0x76, 0xf8, 0x73, 0xe7, 0x80, 0xea, 0x5d, 0x93, 0x9d, 0x61, 0x2a, 0x24,
  0x06, 0x4c, 0x6b, 0x68, 0xea, 0x63, 0x1d, 0x3a, 0xce, 0x8b, 0x31, 0x05,
  0x72, 0xe9, 0xdc, 0xce, 0x05, 0x8c, 0x7a, 0xc1, 0xf5, 0xd6, 0xe1, 0xd0,
  0x6d, 0x73, 0x4e, 0xc1, 0x8a, 0x16, 0x49, 0x4d, 0xac, 0x21, 0xfb, 0x2d,
  0x12, 0xdd, 0x1e, 0xef, 0x9b, 0xb3, 0x6d, 0x20, 0x69, 0xfb, 0xaa, 0x3c,
  0x2c, 0x8f, 0x0d, 0x7c, 0x09, 0x1c, 0x1a, 0xa5, 0x23, 0x83, 0x98, 0x10,
  0xf4, 0x0c, 0x78, 0xa4, 0xa0, 0x4b, 0x1a, 0x71, 0x52, 0x4b, 0xb3, 0x8b,
  0x4c, 0x29, 0xfe, 0x6f, 0x24, 0x88, 0x7e, 0xd6, 0xf0, 0x6a, 0x2c, 0x3c,
  0x6c, 0xad, 0x54, 0x0c, 0xb6, 0x7b, 0xa4, 0x76, 0x38, 0xbf, 0x35, 0xc6,
  0x66, 0xde, 0x6a, 0x92, 0x43, 0x26, 0x31, 0xae, 0x76, 0xb5, 0xf6, 0x15,
  0x87, 0x02, 0x37, 0x7e, 0x7b, 0x92, 0xda, 0x3f, 0x45, 0x1e, 0x89, 0xb7,
  0x26, 0xb3, 0x8d, 0x3c, 0xf6, 0x49, 0x25, 0xe9, 0x1e, 0x52, 0x6f, 0x40,
  0x2e, 0xb6, 0x7d, 0x5c, 0x2a, 0x8a, 0x9c, 0x7d, 0x2c, 0x09, 0xce, 0xde,
  0x73, 0x5d, 0x3b, 0x7d, 0x4f, 0x3f, 0x4b, 0xdc, 0xeb, 0x8b, 0x93, 0xd1,
  0xc2, 0xf3, 0x29, 0x08, 0x98, 0x06, 0x8c, 0x46, 0x38, 0xe1, 0xd5, 0x30,
  0xc1, 0xf0, 0x3c, 0x74, 0x58, 0xf0, 0x20, 0x8b, 0xea, 0x30, 0x43, 0xd8,
  0x3d, 0x19, 0xc4, 0xd1, 0x04, 0xc8, 0xe5, 0x76, 0x31, 0x36, 0x4a, 0xae,
  0xf4, 0x21, 0x41, 0x0b, 0x02, 0x03, 0x01, 0x00, 0x01, 0x02, 0x82, 0x01,
  0x00, 0x7b, 0xe7, 0x87, 0x58, 0x54, 0x59, 0x84, 0xf1, 0x8c, 0x92, 0x72,
  0xa1, 0xd2, 0xf7, 0x5a, 0xbb, 0x98, 0xa5, 0x35, 0x3b, 0x3e, 0xda, 0x66,
  0x2d, 0xdc, 0xcc, 0xc6, 0x9e, 0xdf, 0x73, 0xc4, 0xa7, 0xb2, 0x09, 0x2c,
  0x31, 0x77, 0x72, 0x57, 0x27, 0xc0, 0x48, 0x86, 0x9a, 0x05, 0x1f, 0x96,
  0x3f, 0x40, 0x36, 0x37, 0x92, 0xf3, 0xce, 0xfa, 0x6a, 0x11, 0xbd, 0x44,
  0x30, 0x6d, 0xbd, 0x00, 0xd1, 0x82, 0xb0, 0x77, 0xaf, 0xdb, 0x44, 0x74,
  0x97, 0xb7, 0x57, 0x73, 0x8e, 0x9a, 0x20, 0xab, 0x56, 0x40, 0x79, 0xd9,
  0x63, 0xd7, 0xf4, 0xeb, 0xa1, 0x3f, 0x1c, 0xe1, 0xd4, 0xb4, 0x37, 0xce,
  0xf6, 0xbf, 0xe6, 0x07, 0x06, 0x81, 0x55, 0x69, 0x59, 0x23, 0xc5, 0xaf,
  0xa9, 0x62, 0xa6, 0x17, 0x84, 0xd7, 0x40, 0xa1, 0xa8, 0xfe, 0xae, 0x4c,
  0x3e, 0xa2, 0xe0, 0x9a, 0x27, 0x39, 0x33, 0xf4, 0xd7, 0x62, 0x94, 0xac,
  0x58, 0xb8, 0x45, 0x03, 0xdf, 0x19, 0x21, 0x5b, 0xf8, 0x0c, 0xfb, 0x44,
  0x7e, 0xc7, 0x94, 0x15, 0xc1, 0xb5, 0x34, 0xfe, 0x1a, 0x35, 0xf7, 0x73,
  0x7a, 0xf4, 0x5f, 0x15, 0xaf, 0x6e, 0x93, 0x46, 0xbc, 0xae, 0x16, 0x32,
  0xed, 0x37, 0x73, 0x69, 0x17, 0xd7, 0xcc, 0x41, 0x6a, 0x25, 0x17, 0xa6,
  0x35, 0x58, 0x46, 0xb6, 0x2f, 0x13, 0x64, 0x1b, 0x04, 0x02, 0xdc, 0x5d,
  0x06, 0xe6, 0xf3, 0xb8, 0xf5, 0x93, 0xee, 0x39, 0xff, 0x0a, 0xa3, 0x33,
  0x9b, 0x30, 0x63, 0x5e, 0xf8, 0xfd, 0xd6, 0x45, 0x56, 0xe8, 0xdf, 0x64,
  0x62, 0x23, 0xa0, 0xf1, 0x1e, 0x6a, 0x4c, 0xc0, 0xc1, 0x95, 0x47, 0x05,
  0xb7, 0x5a, 0xb3, 0xda, 0xa5, 0x91, 0x81, 0x49, 0xef, 0x1d, 0x36, 0x14,
  0x55, 0x0f, 0x00, 0x9e, 0x28, 0x1b, 0x15, 0x77, 0x6c, 0x24, 0xd0, 0xe8,
  0x3f, 0x79, 0x37, 0x6a, 0xd1, 0x02, 0x81, 0x81, 0x00, 0xdb, 0x95, 0x25,
  0xee, 0x4f, 0x77, 0x20, 0xcf, 0xf8, 0xc4, 0x3a, 0xa5, 0xcf, 0x15, 0x28,
  0x4f, 0xdf, 0x16, 0xbb, 0xfc, 0x10, 0xaa, 0xa5, 0x7f, 0x0d, 0xea, 0x93,
  0xe2, 0xaf, 0x91, 0x9c, 0x77, 0xcc, 0x51, 0x8e, 0xb0, 0x79, 0xc8, 0xba,
  0xe4, 0x75, 0xcb, 0x07, 0x47, 0x04, 0xab, 0xb5, 0xd0, 0x75, 0x6f, 0x61,
  0xa4, 0x98, 0x2d, 0x15, 0xa9, 0x8a, 0xa2, 0x44, 0xde, 0x4c, 0xd7, 0x7f,
  0x01, 0xb7, 0xb4, 0x71, 0x21, 0x6e, 0xff, 0x0b, 0x15, 0x46, 0x81, 0xd8,
  0x53, 0x11, 0x1d, 0x7c, 0x8f, 0x48, 0x6a, 0xd0, 0x1c, 0xe8, 0xe0, 0x7d,
  0xed, 0x44, 0x17, 0x30, 0xcb, 0xed, 0xfc, 0x65, 0x92, 0x21, 0x62, 0x75,
  0x1f, 0x5c, 0x6a, 0x32, 0x4f, 0xc9, 0xb8, 0x98, 0x11, 0xc7, 0x54, 0x18,
  ..    ..    ..    ..    ..    ..   ..    ..    ..    ..    ..    ..
  0x3b, 0x25, 0x36, 0xc2, 0xcd, 0x02, 0x81, 0x81, 0x00, 0xd5, 0x05, 0x4a,
  0x96, 0xf5, 0x50, 0xc4, 0x46, 0x95*/
};


static const GSMRootCert SECRET_GSM_ROOT_CERTS[] = 
{
  {
    "DST_Root_CA_X3",
    (const uint8_t[])
    {
      0x30, 0x82, 0x03, 0x4a, 0x30, 0x82, 0x02, 0x32, 0xa0, 0x03, 0x02, 0x01,
      0x02, 0x02, 0x10, 0x44, 0xaf, 0xb0, 0x80, 0xd6, 0xa3, 0x27, 0xba, 0x89,
      0x30, 0x39, 0x86, 0x2e, 0xf8, 0x40, 0x6b, 0x30, 0x0d, 0x06, 0x09, 0x2a,
      0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x05, 0x05, 0x00, 0x30, 0x3f,
      0x31, 0x24, 0x30, 0x22, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13, 0x1b, 0x44,
      0x69, 0x67, 0x69, 0x74, 0x61, 0x6c, 0x20, 0x53, 0x69, 0x67, 0x6e, 0x61,
      0x74, 0x75, 0x72, 0x65, 0x20, 0x54, 0x72, 0x75, 0x73, 0x74, 0x20, 0x43,
      0x6f, 0x2e, 0x31, 0x17, 0x30, 0x15, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13,
      0x0e, 0x44, 0x53, 0x54, 0x20, 0x52, 0x6f, 0x6f, 0x74, 0x20, 0x43, 0x41,
      0x20, 0x58, 0x33, 0x30, 0x1e, 0x17, 0x0d, 0x30, 0x30, 0x30, 0x39, 0x33,
      0x30, 0x32, 0x31, 0x31, 0x32, 0x31, 0x39, 0x5a, 0x17, 0x0d, 0x32, 0x31,
      0x30, 0x39, 0x33, 0x30, 0x31, 0x34, 0x30, 0x31, 0x31, 0x35, 0x5a, 0x30,
      0x3f, 0x31, 0x24, 0x30, 0x22, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13, 0x1b,
      0x44, 0x69, 0x67, 0x69, 0x74, 0x61, 0x6c, 0x20, 0x53, 0x69, 0x67, 0x6e,
      0x61, 0x74, 0x75, 0x72, 0x65, 0x20, 0x54, 0x72, 0x75, 0x73, 0x74, 0x20,
      0x43, 0x6f, 0x2e, 0x31, 0x17, 0x30, 0x15, 0x06, 0x03, 0x55, 0x04, 0x03,
      0x13, 0x0e, 0x44, 0x53, 0x54, 0x20, 0x52, 0x6f, 0x6f, 0x74, 0x20, 0x43,
      0x41, 0x20, 0x58, 0x33, 0x30, 0x82, 0x01, 0x22, 0x30, 0x0d, 0x06, 0x09,
      0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03,
      0x82, 0x01, 0x0f, 0x00, 0x30, 0x82, 0x01, 0x0a, 0x02, 0x82, 0x01, 0x01,
      0x00, 0xdf, 0xaf, 0xe9, 0x97, 0x50, 0x08, 0x83, 0x57, 0xb4, 0xcc, 0x62,
      0x65, 0xf6, 0x90, 0x82, 0xec, 0xc7, 0xd3, 0x2c, 0x6b, 0x30, 0xca, 0x5b,
      0xec, 0xd9, 0xc3, 0x7d, 0xc7, 0x40, 0xc1, 0x18, 0x14, 0x8b, 0xe0, 0xe8,
      0x33, 0x76, 0x49, 0x2a, 0xe3, 0x3f, 0x21, 0x49, 0x93, 0xac, 0x4e, 0x0e,
      0xaf, 0x3e, 0x48, 0xcb, 0x65, 0xee, 0xfc, 0xd3, 0x21, 0x0f, 0x65, 0xd2,
      0x2a, 0xd9, 0x32, 0x8f, 0x8c, 0xe5, 0xf7, 0x77, 0xb0, 0x12, 0x7b, 0xb5,
      0x95, 0xc0, 0x89, 0xa3, 0xa9, 0xba, 0xed, 0x73, 0x2e, 0x7a, 0x0c, 0x06,
      0x32, 0x83, 0xa2, 0x7e, 0x8a, 0x14, 0x30, 0xcd, 0x11, 0xa0, 0xe1, 0x2a,
      0x38, 0xb9, 0x79, 0x0a, 0x31, 0xfd, 0x50, 0xbd, 0x80, 0x65, 0xdf, 0xb7,
      0x51, 0x63, 0x83, 0xc8, 0xe2, 0x88, 0x61, 0xea, 0x4b, 0x61, 0x81, 0xec,
      0x52, 0x6b, 0xb9, 0xa2, 0xe2, 0x4b, 0x1a, 0x28, 0x9f, 0x48, 0xa3, 0x9e,
      0x0c, 0xda, 0x09, 0x8e, 0x3e, 0x17, 0x2e, 0x1e, 0xdd, 0x20, 0xdf, 0x5b,
      0xc6, 0x2a, 0x8a, 0xab, 0x2e, 0xbd, 0x70, 0xad, 0xc5, 0x0b, 0x1a, 0x25,
      0x90, 0x74, 0x72, 0xc5, 0x7b, 0x6a, 0xab, 0x34, 0xd6, 0x30, 0x89, 0xff,
      0xe5, 0x68, 0x13, 0x7b, 0x54, 0x0b, 0xc8, 0xd6, 0xae, 0xec, 0x5a, 0x9c,
      0x92, 0x1e, 0x3d, 0x64, 0xb3, 0x8c, 0xc6, 0xdf, 0xbf, 0xc9, 0x41, 0x70,
      0xec, 0x16, 0x72, 0xd5, 0x26, 0xec, 0x38, 0x55, 0x39, 0x43, 0xd0, 0xfc,
      0xfd, 0x18, 0x5c, 0x40, 0xf1, 0x97, 0xeb, 0xd5, 0x9a, 0x9b, 0x8d, 0x1d,
      0xba, 0xda, 0x25, 0xb9, 0xc6, 0xd8, 0xdf, 0xc1, 0x15, 0x02, 0x3a, 0xab,
      0xda, 0x6e, 0xf1, 0x3e, 0x2e, 0xf5, 0x5c, 0x08, 0x9c, 0x3c, 0xd6, 0x83,
      0x69, 0xe4, 0x10, 0x9b, 0x19, 0x2a, 0xb6, 0x29, 0x57, 0xe3, 0xe5, 0x3d,
      0x9b, 0x9f, 0xf0, 0x02, 0x5d, 0x02, 0x03, 0x01, 0x00, 0x01, 0xa3, 0x42,
      0x30, 0x40, 0x30, 0x0f, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x01, 0x01, 0xff,
      0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xff, 0x30, 0x0e, 0x06, 0x03, 0x55,
      0x1d, 0x0f, 0x01, 0x01, 0xff, 0x04, 0x04, 0x03, 0x02, 0x01, 0x06, 0x30,
      0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0xc4, 0xa7,
      0xb1, 0xa4, 0x7b, 0x2c, 0x71, 0xfa, 0xdb, 0xe1, 0x4b, 0x90, 0x75, 0xff,
      0xc4, 0x15, 0x60, 0x85, 0x89, 0x10, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86,
      0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x05, 0x05, 0x00, 0x03, 0x82, 0x01,
      0x01, 0x00, 0xa3, 0x1a, 0x2c, 0x9b, 0x17, 0x00, 0x5c, 0xa9, 0x1e, 0xee,
      0x28, 0x66, 0x37, 0x3a, 0xbf, 0x83, 0xc7, 0x3f, 0x4b, 0xc3, 0x09, 0xa0,
      0x95, 0x20, 0x5d, 0xe3, 0xd9, 0x59, 0x44, 0xd2, 0x3e, 0x0d, 0x3e, 0xbd,
      0x8a, 0x4b, 0xa0, 0x74, 0x1f, 0xce, 0x10, 0x82, 0x9c, 0x74, 0x1a, 0x1d,
      0x7e, 0x98, 0x1a, 0xdd, 0xcb, 0x13, 0x4b, 0xb3, 0x20, 0x44, 0xe4, 0x91,
      0xe9, 0xcc, 0xfc, 0x7d, 0xa5, 0xdb, 0x6a, 0xe5, 0xfe, 0xe6, 0xfd, 0xe0,
      0x4e, 0xdd, 0xb7, 0x00, 0x3a, 0xb5, 0x70, 0x49, 0xaf, 0xf2, 0xe5, 0xeb,
      0x02, 0xf1, 0xd1, 0x02, 0x8b, 0x19, 0xcb, 0x94, 0x3a, 0x5e, 0x48, 0xc4,
      0x18, 0x1e, 0x58, 0x19, 0x5f, 0x1e, 0x02, 0x5a, 0xf0, 0x0c, 0xf1, 0xb1,
      0xad, 0xa9, 0xdc, 0x59, 0x86, 0x8b, 0x6e, 0xe9, 0x91, 0xf5, 0x86, 0xca,
      0xfa, 0xb9, 0x66, 0x33, 0xaa, 0x59, 0x5b, 0xce, 0xe2, 0xa7, 0x16, 0x73,
      0x47, 0xcb, 0x2b, 0xcc, 0x99, 0xb0, 0x37, 0x48, 0xcf, 0xe3, 0x56, 0x4b,
      0xf5, 0xcf, 0x0f, 0x0c, 0x72, 0x32, 0x87, 0xc6, 0xf0, 0x44, 0xbb, 0x53,
      0x72, 0x6d, 0x43, 0xf5, 0x26, 0x48, 0x9a, 0x52, 0x67, 0xb7, 0x58, 0xab,
      0xfe, 0x67, 0x76, 0x71, 0x78, 0xdb, 0x0d, 0xa2, 0x56, 0x14, 0x13, 0x39,
      0x24, 0x31, 0x85, 0xa2, 0xa8, 0x02, 0x5a, 0x30, 0x47, 0xe1, 0xdd, 0x50,
      0x07, 0xbc, 0x02, 0x09, 0x90, 0x00, 0xeb, 0x64, 0x63, 0x60, 0x9b, 0x16,
      0xbc, 0x88, 0xc9, 0x12, 0xe6, 0xd2, 0x7d, 0x91, 0x8b, 0xf9, 0x3d, 0x32,
      0x8d, 0x65, 0xb4, 0xe9, 0x7c, 0xb1, 0x57, 0x76, 0xea, 0xc5, 0xb6, 0x28,
      0x39, 0xbf, 0x15, 0x65, 0x1c, 0xc8, 0xf6, 0x77, 0x96, 0x6a, 0x0a, 0x8d,
      0x77, 0x0b, 0xd8, 0x91, 0x0b, 0x04, 0x8e, 0x07, 0xdb, 0x29, 0xb6, 0x0a,
      0xee, 0x9d, 0x82, 0x35, 0x35, 0x10
    },
    846
  },
  {
    "Let_s_Encrypt_Authority_X3",
    (const uint8_t[])
    {
      0x30, 0x82, 0x04, 0x92, 0x30, 0x82, 0x03, 0x7a, 0xa0, 0x03, 0x02, 0x01,
      0x02, 0x02, 0x10, 0x0a, 0x01, 0x41, 0x42, 0x00, 0x00, 0x01, 0x53, 0x85,
      0x73, 0x6a, 0x0b, 0x85, 0xec, 0xa7, 0x08, 0x30, 0x0d, 0x06, 0x09, 0x2a,
      0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05, 0x00, 0x30, 0x3f,
      0x31, 0x24, 0x30, 0x22, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13, 0x1b, 0x44,
      0x69, 0x67, 0x69, 0x74, 0x61, 0x6c, 0x20, 0x53, 0x69, 0x67, 0x6e, 0x61,
      0x74, 0x75, 0x72, 0x65, 0x20, 0x54, 0x72, 0x75, 0x73, 0x74, 0x20, 0x43,
      0x6f, 0x2e, 0x31, 0x17, 0x30, 0x15, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13,
      0x0e, 0x44, 0x53, 0x54, 0x20, 0x52, 0x6f, 0x6f, 0x74, 0x20, 0x43, 0x41,
      0x20, 0x58, 0x33, 0x30, 0x1e, 0x17, 0x0d, 0x31, 0x36, 0x30, 0x33, 0x31,
      0x37, 0x31, 0x36, 0x34, 0x30, 0x34, 0x36, 0x5a, 0x17, 0x0d, 0x32, 0x31,
      0x30, 0x33, 0x31, 0x37, 0x31, 0x36, 0x34, 0x30, 0x34, 0x36, 0x5a, 0x30,
      0x4a, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02,
      0x55, 0x53, 0x31, 0x16, 0x30, 0x14, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x13,
      0x0d, 0x4c, 0x65, 0x74, 0x27, 0x73, 0x20, 0x45, 0x6e, 0x63, 0x72, 0x79,
      0x70, 0x74, 0x31, 0x23, 0x30, 0x21, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13,
      0x1a, 0x4c, 0x65, 0x74, 0x27, 0x73, 0x20, 0x45, 0x6e, 0x63, 0x72, 0x79,
      0x70, 0x74, 0x20, 0x41, 0x75, 0x74, 0x68, 0x6f, 0x72, 0x69, 0x74, 0x79,
      0x20, 0x58, 0x33, 0x30, 0x82, 0x01, 0x22, 0x30, 0x0d, 0x06, 0x09, 0x2a,
      0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x82,
      0x01, 0x0f, 0x00, 0x30, 0x82, 0x01, 0x0a, 0x02, 0x82, 0x01, 0x01, 0x00,
      0x9c, 0xd3, 0x0c, 0xf0, 0x5a, 0xe5, 0x2e, 0x47, 0xb7, 0x72, 0x5d, 0x37,
      0x83, 0xb3, 0x68, 0x63, 0x30, 0xea, 0xd7, 0x35, 0x26, 0x19, 0x25, 0xe1,
      0xbd, 0xbe, 0x35, 0xf1, 0x70, 0x92, 0x2f, 0xb7, 0xb8, 0x4b, 0x41, 0x05,
      0xab, 0xa9, 0x9e, 0x35, 0x08, 0x58, 0xec, 0xb1, 0x2a, 0xc4, 0x68, 0x87,
      0x0b, 0xa3, 0xe3, 0x75, 0xe4, 0xe6, 0xf3, 0xa7, 0x62, 0x71, 0xba, 0x79,
      0x81, 0x60, 0x1f, 0xd7, 0x91, 0x9a, 0x9f, 0xf3, 0xd0, 0x78, 0x67, 0x71,
      0xc8, 0x69, 0x0e, 0x95, 0x91, 0xcf, 0xfe, 0xe6, 0x99, 0xe9, 0x60, 0x3c,
      0x48, 0xcc, 0x7e, 0xca, 0x4d, 0x77, 0x12, 0x24, 0x9d, 0x47, 0x1b, 0x5a,
      0xeb, 0xb9, 0xec, 0x1e, 0x37, 0x00, 0x1c, 0x9c, 0xac, 0x7b, 0xa7, 0x05,
      0xea, 0xce, 0x4a, 0xeb, 0xbd, 0x41, 0xe5, 0x36, 0x98, 0xb9, 0xcb, 0xfd,
      0x6d, 0x3c, 0x96, 0x68, 0xdf, 0x23, 0x2a, 0x42, 0x90, 0x0c, 0x86, 0x74,
      0x67, 0xc8, 0x7f, 0xa5, 0x9a, 0xb8, 0x52, 0x61, 0x14, 0x13, 0x3f, 0x65,
      0xe9, 0x82, 0x87, 0xcb, 0xdb, 0xfa, 0x0e, 0x56, 0xf6, 0x86, 0x89, 0xf3,
      0x85, 0x3f, 0x97, 0x86, 0xaf, 0xb0, 0xdc, 0x1a, 0xef, 0x6b, 0x0d, 0x95,
      0x16, 0x7d, 0xc4, 0x2b, 0xa0, 0x65, 0xb2, 0x99, 0x04, 0x36, 0x75, 0x80,
      0x6b, 0xac, 0x4a, 0xf3, 0x1b, 0x90, 0x49, 0x78, 0x2f, 0xa2, 0x96, 0x4f,
      0x2a, 0x20, 0x25, 0x29, 0x04, 0xc6, 0x74, 0xc0, 0xd0, 0x31, 0xcd, 0x8f,
      0x31, 0x38, 0x95, 0x16, 0xba, 0xa8, 0x33, 0xb8, 0x43, 0xf1, 0xb1, 0x1f,
      0xc3, 0x30, 0x7f, 0xa2, 0x79, 0x31, 0x13, 0x3d, 0x2d, 0x36, 0xf8, 0xe3,
      0xfc, 0xf2, 0x33, 0x6a, 0xb9, 0x39, 0x31, 0xc5, 0xaf, 0xc4, 0x8d, 0x0d,
      0x1d, 0x64, 0x16, 0x33, 0xaa, 0xfa, 0x84, 0x29, 0xb6, 0xd4, 0x0b, 0xc0,
      0xd8, 0x7d, 0xc3, 0x93, 0x02, 0x03, 0x01, 0x00, 0x01, 0xa3, 0x82, 0x01,
      0x7d, 0x30, 0x82, 0x01, 0x79, 0x30, 0x12, 0x06, 0x03, 0x55, 0x1d, 0x13,
      0x01, 0x01, 0xff, 0x04, 0x08, 0x30, 0x06, 0x01, 0x01, 0xff, 0x02, 0x01,
      0x00, 0x30, 0x0e, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x01, 0x01, 0xff, 0x04,
      0x04, 0x03, 0x02, 0x01, 0x86, 0x30, 0x7f, 0x06, 0x08, 0x2b, 0x06, 0x01,
      0x05, 0x05, 0x07, 0x01, 0x01, 0x04, 0x73, 0x30, 0x71, 0x30, 0x32, 0x06,
      0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x30, 0x01, 0x86, 0x26, 0x68,
      0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x69, 0x73, 0x72, 0x67, 0x2e, 0x74,
      0x72, 0x75, 0x73, 0x74, 0x69, 0x64, 0x2e, 0x6f, 0x63, 0x73, 0x70, 0x2e,
      0x69, 0x64, 0x65, 0x6e, 0x74, 0x72, 0x75, 0x73, 0x74, 0x2e, 0x63, 0x6f,
      0x6d, 0x30, 0x3b, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x30,
      0x02, 0x86, 0x2f, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x61, 0x70,
      0x70, 0x73, 0x2e, 0x69, 0x64, 0x65, 0x6e, 0x74, 0x72, 0x75, 0x73, 0x74,
      0x2e, 0x63, 0x6f, 0x6d, 0x2f, 0x72, 0x6f, 0x6f, 0x74, 0x73, 0x2f, 0x64,
      0x73, 0x74, 0x72, 0x6f, 0x6f, 0x74, 0x63, 0x61, 0x78, 0x33, 0x2e, 0x70,
      0x37, 0x63, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04, 0x18, 0x30,
      0x16, 0x80, 0x14, 0xc4, 0xa7, 0xb1, 0xa4, 0x7b, 0x2c, 0x71, 0xfa, 0xdb,
      0xe1, 0x4b, 0x90, 0x75, 0xff, 0xc4, 0x15, 0x60, 0x85, 0x89, 0x10, 0x30,
      0x54, 0x06, 0x03, 0x55, 0x1d, 0x20, 0x04, 0x4d, 0x30, 0x4b, 0x30, 0x08,
      0x06, 0x06, 0x67, 0x81, 0x0c, 0x01, 0x02, 0x01, 0x30, 0x3f, 0x06, 0x0b,
      0x2b, 0x06, 0x01, 0x04, 0x01, 0x82, 0xdf, 0x13, 0x01, 0x01, 0x01, 0x30,
      0x30, 0x30, 0x2e, 0x06, 0x08, 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x02,
      0x01, 0x16, 0x22, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x63, 0x70,
      0x73, 0x2e, 0x72, 0x6f, 0x6f, 0x74, 0x2d, 0x78, 0x31, 0x2e, 0x6c, 0x65,
      0x74, 0x73, 0x65, 0x6e, 0x63, 0x72, 0x79, 0x70, 0x74, 0x2e, 0x6f, 0x72,
      0x67, 0x30, 0x3c, 0x06, 0x03, 0x55, 0x1d, 0x1f, 0x04, 0x35, 0x30, 0x33,
      0x30, 0x31, 0xa0, 0x2f, 0xa0, 0x2d, 0x86, 0x2b, 0x68, 0x74, 0x74, 0x70,
      0x3a, 0x2f, 0x2f, 0x63, 0x72, 0x6c, 0x2e, 0x69, 0x64, 0x65, 0x6e, 0x74,
      0x72, 0x75, 0x73, 0x74, 0x2e, 0x63, 0x6f, 0x6d, 0x2f, 0x44, 0x53, 0x54,
      0x52, 0x4f, 0x4f, 0x54, 0x43, 0x41, 0x58, 0x33, 0x43, 0x52, 0x4c, 0x2e,
      0x63, 0x72, 0x6c, 0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16,
      0x04, 0x14, 0xa8, 0x4a, 0x6a, 0x63, 0x04, 0x7d, 0xdd, 0xba, 0xe6, 0xd1,
      0x39, 0xb7, 0xa6, 0x45, 0x65, 0xef, 0xf3, 0xa8, 0xec, 0xa1, 0x30, 0x0d,
      0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0b, 0x05,
      0x00, 0x03, 0x82, 0x01, 0x01, 0x00, 0xdd, 0x33, 0xd7, 0x11, 0xf3, 0x63,
      0x58, 0x38, 0xdd, 0x18, 0x15, 0xfb, 0x09, 0x55, 0xbe, 0x76, 0x56, 0xb9,
      0x70, 0x48, 0xa5, 0x69, 0x47, 0x27, 0x7b, 0xc2, 0x24, 0x08, 0x92, 0xf1,
      0x5a, 0x1f, 0x4a, 0x12, 0x29, 0x37, 0x24, 0x74, 0x51, 0x1c, 0x62, 0x68,
      0xb8, 0xcd, 0x95, 0x70, 0x67, 0xe5, 0xf7, 0xa4, 0xbc, 0x4e, 0x28, 0x51,
      0xcd, 0x9b, 0xe8, 0xae, 0x87, 0x9d, 0xea, 0xd8, 0xba, 0x5a, 0xa1, 0x01,
      0x9a, 0xdc, 0xf0, 0xdd, 0x6a, 0x1d, 0x6a, 0xd8, 0x3e, 0x57, 0x23, 0x9e,
      0xa6, 0x1e, 0x04, 0x62, 0x9a, 0xff, 0xd7, 0x05, 0xca, 0xb7, 0x1f, 0x3f,
      0xc0, 0x0a, 0x48, 0xbc, 0x94, 0xb0, 0xb6, 0x65, 0x62, 0xe0, 0xc1, 0x54,
      0xe5, 0xa3, 0x2a, 0xad, 0x20, 0xc4, 0xe9, 0xe6, 0xbb, 0xdc, 0xc8, 0xf6,
      0xb5, 0xc3, 0x32, 0xa3, 0x98, 0xcc, 0x77, 0xa8, 0xe6, 0x79, 0x65, 0x07,
      0x2b, 0xcb, 0x28, 0xfe, 0x3a, 0x16, 0x52, 0x81, 0xce, 0x52, 0x0c, 0x2e,
      0x5f, 0x83, 0xe8, 0xd5, 0x06, 0x33, 0xfb, 0x77, 0x6c, 0xce, 0x40, 0xea,
      0x32, 0x9e, 0x1f, 0x92, 0x5c, 0x41, 0xc1, 0x74, 0x6c, 0x5b, 0x5d, 0x0a,
      0x5f, 0x33, 0xcc, 0x4d, 0x9f, 0xac, 0x38, 0xf0, 0x2f, 0x7b, 0x2c, 0x62,
      0x9d, 0xd9, 0xa3, 0x91, 0x6f, 0x25, 0x1b, 0x2f, 0x90, 0xb1, 0x19, 0x46,
      0x3d, 0xf6, 0x7e, 0x1b, 0xa6, 0x7a, 0x87, 0xb9, 0xa3, 0x7a, 0x6d, 0x18,
      0xfa, 0x25, 0xa5, 0x91, 0x87, 0x15, 0xe0, 0xf2, 0x16, 0x2f, 0x58, 0xb0,
      0x06, 0x2f, 0x2c, 0x68, 0x26, 0xc6, 0x4b, 0x98, 0xcd, 0xda, 0x9f, 0x0c,
      0xf9, 0x7f, 0x90, 0xed, 0x43, 0x4a, 0x12, 0x44, 0x4e, 0x6f, 0x73, 0x7a,
      0x28, 0xea, 0xa4, 0xaa, 0x6e, 0x7b, 0x4c, 0x7d, 0x87, 0xdd, 0xe0, 0xc9,
      0x02, 0x44, 0xa7, 0x87, 0xaf, 0xc3, 0x34, 0x5b, 0xb4, 0x42
    },
    1174
  }
};

#define SECRET_GSM_ROOT_SIZE  (sizeof(SECRET_GSM_ROOT_CERTS)/sizeof(SECRET_GSM_ROOT_CERTS[0]))

#endif    //defines_h
