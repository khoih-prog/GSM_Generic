/*********************************************************************************************************************************
  SerialGSMSPassthrough.ino
  
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
 
  Version: 1.2.4
  
  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.2.4    K Hoang     11/03/2021 Initial public release to add support to many boards / modules besides MKRGSM 1400 / SARA U201
 **********************************************************************************************************************************/
/*
   SerialGSMPassthrough sketch

   This sketch allows you to send AT commands from the USB CDC serial port
   of the GSM module to the onboard ublox SARA-U201 celluar module.

   For a list of supported AT commands see:
   https://www.u-blox.com/sites/default/files/u-blox-CEL_ATCommands_%28UBX-13002752%29.pdf

   Circuit:
   - GSM module
   - Antenna
   - 1500 mAh or higher LIPO battery connected
   - SIM card

   Make sure the Serial Monitor's line ending is set to "Both NL and CR"

   create 11 December 2017
   Sandeep Mistry
*/

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

// baud rate used for both Serial ports
unsigned long baud = 115200;

void setup() 
{
  // reset the ublox module
  pinMode(GSM_RESETN, OUTPUT);
  digitalWrite(GSM_RESETN, HIGH);
  delay(100);
  digitalWrite(GSM_RESETN, LOW);

  Serial.begin(baud);
  SerialGSM.begin(baud);
}

void loop() 
{
  if (Serial.available()) 
  {
    SerialGSM.write(Serial.read());
  }

  if (SerialGSM.available()) 
  {
    Serial.write(SerialGSM.read());
  }
}
