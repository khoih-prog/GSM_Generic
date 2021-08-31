/*********************************************************************************************************************************
  ChooseRadioAccessTechnology.ino
  
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
  Radio Access Technology selection for GSM module

  This sketch allows you to select your preferred Radio Access Technology (RAT).

  You can choose among 2G, 3G/UTMS or a combination of both.

  The chosen configuration will be saved to modem's internal memory and will be preserved through main board sketch uploads.

  In order to change the RAT, you will need to run this sketch again.

  Circuit:
  - GSM module
  - Antenna
  - SIM card

  Created 19 August 2019
  by Riccardo Rizzo

*/

#define DEBUG_GSM_GENERIC_PORT       Serial

// Debug Level from 0 to 5. Level 5 is to print out AT commands and responses
#define _GSM_GENERIC_LOGLEVEL_       5

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

// BaudRate to communicate to GSM/GPRS modem. If be limit to max 115200 inside modem
unsigned long baudRateSerialGSM  = 115200;

// libraries
#include <GSM_Generic_Main.h>

bool setRAT(String choice)
{
  String response;

  Serial.print("Disconnecting from network: ");

  MODEM.deregisterCarrier();

  MODEM.waitForResponse(10000);
  Serial.println("done.");

  Serial.print("Setting Radio Access Technology: ");

  setRAT(choice.c_str());
  
  Serial.println("done.");

  return true;
}

bool apply()
{
  Serial.print("Applying changes and saving configuration: ");
  MODEM.reset();
  delay(5000);
  Serial.println("Modem restart");
  
  MODEM.begin(baudRateSerialGSM, true);

  do 
  {
    delay(1000);
    MODEM.noop();
  } while (MODEM.waitForResponse(1000) != GSM_RESPONSE_OK);

  Serial.println("done.");
  
  return true;
}

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial);

  delay(200);

  Serial.print(F("\nStarting ChooseRadioAccessTechnology on ")); Serial.println(BOARD_NAME);
  Serial.println(GSM_GENERIC_VERSION);

#if ( defined(DEBUG_GSM_GENERIC_PORT) && (_GSM_GENERIC_LOGLEVEL_ > 4) )
  MODEM.debug(DEBUG_GSM_GENERIC_PORT);
#endif  

  MODEM.begin(baudRateSerialGSM);
  
  while (!MODEM.noop());

  for (int i = 0; i < 80; i++) 
    Serial.print("*");
    
  Serial.println();
  Serial.println("This sketch allows you to select your preferred");
  Serial.println("GSM Radio Access Technology (RAT).");
  Serial.println();
  Serial.println("You can choose among 2G, 3G/UMTS or a combination of both.");
  Serial.println();
  Serial.println("The chosen configuration will be saved to modem's internal memory");
  Serial.println("and will be preserved through sketch uploads.");
  Serial.println();
  Serial.println("In order to change the RAT, you will need to run this sketch again.");
  
  for (int i = 0; i < 80; i++) 
    Serial.print("*");

  Serial.println();
  Serial.println();
  Serial.println("Please choose your Radio Access Technology:");
  Serial.println();
  Serial.println("    0 - 2G only");
  Serial.println("    1 - 3G/UMTS only");
  Serial.println("    2 - 2G preferred, 3G/UMTS as failover");
  Serial.println("    3 - 3G/UMTS preferred, 2G as failover (default)");
  Serial.println();
}

void loop() 
{
  String uratChoice;

  Serial.print("> ");

  Serial.setTimeout(-1);
  
  while (Serial.available() == 0);
  
  String uratInput = Serial.readStringUntil('\n');
  uratInput.trim();
  int urat = uratInput.toInt();
  
  Serial.println(urat);

  switch (urat) 
  {
    case 0:
      uratChoice = GSM_GPRS_SINGLE_MODE;  //"0";
      break;
    case 1:
      uratChoice = UMTS_SINGLE_MODE;      //"2";
      break;
    case 2:
      uratChoice = GSM_BANDS;             //"1,0";
      break;
    case 3:
      uratChoice = UMTS_BANDS;            //"1,2";
      break;
    default:
      Serial.println("Invalid input. Please, retry.");
      return;
  }

  setRAT(uratChoice);
  apply();

  Serial.println();
  Serial.println("Radio Access Technology selected.");
  Serial.println("Now you can upload your 2G or 3G application sketch.");
  
  while (true);
}
