/*********************************************************************************************************************************
  GSMSSLWebClient.ino
  
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
  Web client

  This sketch connects to a website using SSL through GSM module. Specifically, this example downloads the URL 
  "http://www.arduino.cc/asciilogo.txt" and prints it to the Serial monitor.

  Circuit:
   GSM Module
   Antenna
   SIM card with a data plan

  created 8 Mar 2012
  by Tom Igoe
*/

#include "defines.h"

// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[]     = SECRET_PINNUMBER;
// APN data
const char GPRS_APN[]      = SECRET_GPRS_APN;
const char GPRS_LOGIN[]    = SECRET_GPRS_LOGIN;
const char GPRS_PASSWORD[] = SECRET_GPRS_PASSWORD;

// BaudRate to communicate to GSM/GPRS modem. If be limit to max 115200 inside modem
int baudRateSerialGSM  = 115200;

// initialize the library instance
GSMSSLClient client;
GPRS gprs;
GSM gsmAccess;

// URL, path and port (for example: arduino.cc)
char server[] = "www.cloudflare.com";
char path[]   = "/cdn-cgi/trace";
int port      = 443; // port 443 is the default for HTTPS

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial);

  delay(200);

  Serial.print(F("\nStarting GSMSSLWebClient on ")); Serial.println(BOARD_NAME);
  Serial.println(GSM_GENERIC_VERSION);

  // connection state
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

  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  // Use this to load Root Certs the first time
  // if (client.connect(server, port)) 
  // Use this to not load Root Certs anymore as done last time and stored in modem's flash
  if (client.connect(server, port, false)) 
  {
    Serial.println("connected");
    // Make a HTTP request:
    client.print("GET ");
    client.print(path);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();
  } 
  else 
  {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

void loop() 
{
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available()) 
  {
    char c = client.read();
    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!client.available() && !client.connected()) 
  {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    while (true);
  }
}
