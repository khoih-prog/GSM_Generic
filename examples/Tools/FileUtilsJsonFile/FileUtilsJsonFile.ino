/*********************************************************************************************************************************
  FileUtilsJsonFile.ino

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
  Manage JSON configurations via GSM module filesystem.

  This sketch demonstrates how to use the internal filesystem
  of the GSM module to store and retrieve data and configurations
  both as JSON objects or C++ structs.

  Circuit:
    MKR GSM 1400 board

  created 19 June 2020
  by Giampaolo Mancini
*/

#include <Arduino_JSON.h>

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

// BaudRate to communicate to GSM/GPRS modem. If be limit to max 115200 inside modem
unsigned long baudRateSerialGSM  = 115200;

// libraries
#include <GSM_Generic_Main.h>

#include "Config.h"

GSMFileUtils fileUtils;


void simpleDemo()
{

  Serial.println();
  Serial.println("========================");
  Serial.println("Running simple JSON demo");
  Serial.println();

  JSONVar myObject;

  myObject["hello"] = "world";
  myObject["true"] = true;
  myObject["x"] = 42;

  String jsonString = JSON.stringify(myObject);

  Serial.println("Saving JSON file (test.json): ");
  Serial.println(jsonString);
  Serial.println();
  
  fileUtils.downloadFile("test.json", jsonString);

  printFiles(fileUtils);
  Serial.println();

  String jsonData;
  Serial.println("Reading JSON file (test.json): ");
  
  fileUtils.readFile("test.json", &jsonData);

  Serial.println("File contents:");
  Serial.println(jsonData);
  Serial.println();

  Serial.println("Parsing JSON contents:");
  
  JSONVar myConf = JSON.parse(jsonData);
  Serial.print("myConf[\"hello\"]: ");
  Serial.println(myConf["hello"]);
  Serial.print("myConf[\"true\"]: ");
  Serial.println(myConf["true"]);
  Serial.print("myConf[\"x\"]: ");
  Serial.println(myConf["x"]);
}

void structDemo()
{
  GSMModem modem;

  Serial.println();
  Serial.println("========================");
  Serial.println("Running Configuration via struct and JSON demo");
  Serial.println();
  Serial.println("Creating configuration struct:");

  Config conf;
  conf.deviceId = modem.getICCID();
  conf.timestamp = millis();

  Serial.print(conf);

  fileUtils.downloadFile("conf.json", conf.toJson());

  Serial.println();
  printFiles(fileUtils);
  Serial.println();

  Serial.println("Reading configuration file:");

  String jsonConf;
  
  fileUtils.readFile("conf.json", &jsonConf);
  Serial.println(jsonConf);
  Serial.println();

  Serial.println("Reading configuration struct:");
  Config newConf;
  newConf.fromJSON(jsonConf);

  Serial.print(newConf);
}

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial);

  delay(200);

  Serial.print(F("\nStarting FileUtilsJsonFile on ")); Serial.println(BOARD_NAME);
  Serial.println(GSM_GENERIC_VERSION);

  Serial.println("Store and Retrieve JSON data to GSM module storage.");
  Serial.println();

  fileUtils.begin(baudRateSerialGSM);

  simpleDemo();
  structDemo();

  while (true);
}

void loop()
{
}
