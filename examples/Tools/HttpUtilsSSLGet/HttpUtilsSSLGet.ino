/*********************************************************************************************************************************
  HttpUtilsSSLGet.ino
  
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
  This sketch Shows how to use the HTTP command to makes http request to the https end point and stores
  the result in the Sara U201 internal storage

  Circuit:
  - Sara u201 GSM Module

  Created 3 September 2020
  by Riccardo Rizzo

*/

#include <ArduinoMqttClient.h>

#include "defines.h"

const char PINNUMBER[]     = SECRET_PINNUMBER;
// APN data
const char GPRS_APN[]      = SECRET_GPRS_APN;
const char GPRS_LOGIN[]    = SECRET_GPRS_LOGIN;
const char GPRS_PASSWORD[] = SECRET_GPRS_PASSWORD;

// BaudRate to communicate to GSM/GPRS modem. If be limit to max 115200 inside modem
unsigned long baudRateSerialGSM  = 115200;

// initialize the library instance
GSMHttpUtils httpClient;
GPRS gprs;
GSM gsmAccess;

GSMFileUtils fileUtils(false);

// URL, path and port (for example: example.org)
char server[] = "google.com";
char path[]   = "/";
int port      = 443; // port 80 is the default for HTTP

// An existing file
constexpr char filename[] = "get.ffs";


// Read block size
constexpr unsigned int blockSize { 512 };

const long interval = 1000;
unsigned long previousMillis = 0;

int count = 0;

void readFile() 
{
  String fileList[10];
  
  uint32_t size = fileUtils.listFiles(fileList);
  
  for (uint32_t i = 0; i < size && i < 10; i++) 
  {
    if (fileList[i] == filename) 
    {
      Serial.print("file found: ");
      Serial.println(fileList[i]);
    }
  }
}

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial);

  delay(200);

  Serial.print(F("\nStarting HttpUtilsSSLGet on ")); Serial.println(BOARD_NAME);
  Serial.println(GSM_GENERIC_VERSION);

#if ( defined(DEBUG_GSM_GENERIC_PORT) && (_GSM_GENERIC_LOGLEVEL_ > 4) )
  MODEM.debug(DEBUG_GSM_GENERIC_PORT);
#endif  

  bool connected = false;
  
  // After starting the modem with GSM.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while (!connected)
  {
    if ((gsmAccess.begin(baudRateSerialGSM, PINNUMBER) == GSM_READY) &&
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY))
    {
      connected = true;
    } 
    else 
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }
  
  fileUtils.begin(baudRateSerialGSM, false);

  httpClient.enableSSL();
  httpClient.configServer(server, port);
  //httpClient.gsmGet(path, filename);
  httpClient.get(path, filename);
}

void loop() 
{
  if (httpClient.responseStatus())
  {
    Serial.println("received");

    readFile();
    while (1);
  }
}
