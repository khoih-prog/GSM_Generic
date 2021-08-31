/*********************************************************************************************************************************
  GSMWebClient.ino

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

  This sketch connects to a website through a GSM module. Specifically, this example downloads the URL 
  "http://www.example.org/" and prints it to the Serial monitor.

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

// initialize the library instance
GSMClient client;
GPRS gprs;
GSM gsmAccess;

// URL, path and port (for example: example.org)
char server[] = "arduino.cc";         //"example.org";
char path[]   = "/asciilogo.txt";     //"/";
int port      = 80; // port 80 is the default for HTTP

// BaudRate to communicate to GSM/GPRS modem. If be limit to max 115200 inside modem
unsigned long baudRateSerialGSM  = 115200;

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial);

  delay(200);

  Serial.print(F("\nStarting GSMWebClient on ")); Serial.println(BOARD_NAME);
  Serial.println(GSM_GENERIC_VERSION);

#if ( defined(DEBUG_GSM_GENERIC_PORT) && (_GSM_GENERIC_LOGLEVEL_ > 4) )
  MODEM.debug(DEBUG_GSM_GENERIC_PORT);
#endif  

  // connection state
  bool connected = false;

#if 1
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
#endif

  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, port)) 
  {
    Serial.println("connected");
    // Make a HTTP request:
    delay(2000);
    
 #if 1
    
    String command = "GET /asciilogo.txt HTTP/1.1\r\nHost: arduino.cc\r\nConnection: close\r\n";
    //String command = "GET /asciilogo.txt HTTP/1.1\nHost: arduino.cc\nConnection: close";
    client.println(command);
    //client.println(F("GET /asciilogo.txt HTTP/1.1"));
    //client.println(F("Host: arduino.cc"));
    //client.println(F("Connection: close"));
    //client.println();
 #else
    client.print("GET ");
    client.print(path);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();
 #endif   
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
    //while(true);
    delay(1000);
  }
}
