/*********************************************************************************************************************************
  TestModem.ino

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
/*

  This example tests to see if the modem of the GSM module is working correctly. You do not need
  a SIM card for this example.

  Circuit:
   GSM module
   Antenna

  Created 12 Jun 2012
  by David del Peral
  modified 21 Nov 2012
  by Tom Igoe
*/

#define DEBUG_GSM_GENERIC_PORT       Serial

// Debug Level from 0 to 6. Level 6 is to print out AT commands and responses
#define _GSM_GENERIC_LOGLEVEL_       4

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

#if GSM_USING_SOFTWARE_SERIAL
// BaudRate to communicate to GSM/GPRS modem. If be limit to max 115200 inside modem
// Lower for Software Serial
unsigned long baudRateSerialGSM  = 115200;
#else
// BaudRate to communicate to GSM/GPRS modem. If be limit to max 115200 inside modem
unsigned long baudRateSerialGSM  = 115200;
#endif

// libraries
#include <GSM_Generic_Main.h>

// modem verification object
GSMModem gsmModem;

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial);

#if (ESP8266 && GSM_USING_SOFTWARE_SERIAL)
  SerialGSM.begin(baudRateSerialGSM);
#endif

  delay(200);

  Serial.print(F("\nStarting TestModem on ")); Serial.println(BOARD_NAME);
  Serial.println(GSM_GENERIC_VERSION);

#if ( defined(DEBUG_GSM_GENERIC_PORT) && (_GSM_GENERIC_LOGLEVEL_ > 5) )
  gsmModem.debug(DEBUG_GSM_GENERIC_PORT);
#endif

  if (gsmModem.begin(baudRateSerialGSM))
  {
    Serial.print("gsmModem.begin() succeeded using baudRate = "); Serial.println(baudRateSerialGSM);
  }
  else
  {
    Serial.println("ERROR, no modem answer.");
  }
}

String getIMEI()
{
  // IMEI variable
  String IMEI = "";

  // get modem IMEI
  IMEI = gsmModem.getIMEI();

  // check IMEI response
  if (IMEI != NULL)
  {
    // show IMEI in serial monitor
    Serial.println("Modem's IMEI: " + IMEI);
  }
  else
  {
    Serial.println("Error: Could not get IMEI");
  }

  return IMEI;
}

String getCCID()
{
  // CCID variable
  String CCID = "";

  // get modem CCID
  CCID = gsmModem.getICCID();

  // check CCID response
  if (CCID != NULL)
  {
    // show CCID in serial monitor
    Serial.println("Modem's CCID: " + CCID);
  }
  else
  {
    Serial.println("Error: Could not get CCID");
  }

  return CCID;
}

String getIMSI()
{
  // IMSI
  String IMSI = "";

  // IMSI
  // get modem IMSI
  //IMSI = gsmModem.getIMSI();
  IMSI = MODEM.getIMSI();

  // check IMSI response
  if (IMSI != NULL)
  {
    // show IMSI in serial monitor
    Serial.println("Modem's IMSI: " + IMSI);
  }
  else
  {
    Serial.println("Error: Could not get IMSI");
  }

  return IMSI;
}

void getModemInfo()
{
  Serial.print(F("Modem Name = ")); Serial.println(MODEM.getModemName());
  Serial.print(F("Modem Info = ")); Serial.println(MODEM.getModemInfo());

  getIMEI();
  getCCID();
  getIMSI();
}

void loop()
{
  getModemInfo();

  // reset modem to check booting:
  Serial.println("Resetting modem...");
  gsmModem.begin(baudRateSerialGSM);

  getModemInfo();

  Serial.println("=============================");

  delay(30000);
  //while (true)
  //{
    //delay(1000);
  //}
}
