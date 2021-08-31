/*********************************************************************************************************************************
  TestGPRS.ino
  
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

 This sketch test the GSM module's ability to connect to a GPRS network. It asks for APN information through the
 serial monitor and tries to connect to example.org.

 Circuit:
 * GSM module
 * Antenna
 * SIM card with data plan

 Created 18 Jun 2012
 by David del Peral
*/

#include "defines.h"

#define USING_SERIAL_INPUT      false

// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = SECRET_PINNUMBER;

// BaudRate to communicate to GSM/GPRS modem. If be limit to max 115200 inside modem
unsigned long baudRateSerialGSM  = 115200;

// initialize the library instance
GSM gsmAccess;        // GSM access: include a 'true' parameter for debug enabled
GPRS gprsAccess;  // GPRS access
GSMClient client;  // Client service for TCP connection

// messages for serial monitor response
String oktext = "OK";
String errortext = "ERROR";

// URL and path (for example: example.org)
char url[]      = "example.org";
char urlproxy[] = "http://www.example.org";
char path[]     = "/";

// variable for save response obtained
String response = "";

// use a proxy
bool use_proxy = false;

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial);

  delay(200);

  Serial.print(F("\nStarting TestGPRS on ")); Serial.println(BOARD_NAME);
  Serial.println(GSM_GENERIC_VERSION);

#if ( defined(DEBUG_GSM_GENERIC_PORT) && (_GSM_GENERIC_LOGLEVEL_ > 4) )
  MODEM.debug(DEBUG_GSM_GENERIC_PORT);
#endif  
}

void loop() 
{
  use_proxy = false;

  // start GSM shield
  // if your SIM has PIN, pass it as a parameter of begin() in quotes
  Serial.println("Connecting GSM network...");
  
  if (gsmAccess.begin(baudRateSerialGSM, PINNUMBER) != GSM_READY)
  {
    Serial.println(errortext);
    while (true);
  }
  
  Serial.println(oktext);

  // read APN introduced by user
  
  Serial.print("Enter your APN: ");
  
#if USING_SERIAL_INPUT  
  char apn[50];
  readSerial(apn);
#else
  char apn[] = SECRET_GPRS_APN;
#endif
  
  Serial.println(apn);

  // Read APN login introduced by user
  Serial.print("Now, enter your login: ");

#if USING_SERIAL_INPUT  
  char login[50];
  readSerial(login);
#else
  char login[] = SECRET_GPRS_LOGIN;
#endif  

  Serial.println(login);

  // read APN password introduced by user
  Serial.print("Finally, enter your password: ");

#if USING_SERIAL_INPUT  
  char password[20];
  readSerial(password);
#else
  char password[] = SECRET_GPRS_PASSWORD;
#endif

  Serial.println(password);

  // attach GPRS
  Serial.println("Attaching to GPRS with your APN...");
  
  if (gprsAccess.attachGPRS(apn, login, password) != GPRS_READY) 
  {
    Serial.println(errortext);
  } 
  else 
  {
    Serial.println(oktext);

    // read proxy introduced by user
    Serial.print("If your carrier uses a proxy, enter it, if not press enter: ");

#if USING_SERIAL_INPUT  
    char proxy[100];
    readSerial(proxy);
#else
    char proxy[] = "";
#endif
         
    Serial.println(proxy);

    // connection with example.org and realize HTTP request
    Serial.print("Connecting and sending GET request to ");
    Serial.println(url);
    
    int res_connect;
    // if user introduced a proxy, asks them for proxy port
    int pport;
    
    if (proxy[0] != '\0') 
    {
      // read proxy port introduced by user
      Serial.print("Enter the proxy port: ");

#if USING_SERIAL_INPUT  
      char proxyport[10];
      readSerial(proxyport);
#else
      char proxyport[] = SECRET_GPRS_PASSWORD;
#endif

      Serial.println(proxyport);

      // cast proxy port introduced to integer
      pport = (int) proxyport;
      use_proxy = true;
      Serial.println(proxyport);

      // if use a proxy, connect with it
      res_connect = client.connect(proxy, pport);
    }
    else
    {
      res_connect = client.connect(url, 80);
    }

    if (res_connect) 
    {
      delay(1000);
      
      // make a HTTP 1.0 GET request (client sends the request)
      client.print("GET ");

      // if use a proxy, the path is example.org URL
      if (use_proxy) 
      {
        client.print(urlproxy);
      } 
      else 
      {
        client.print(path);
      }

      //client.println(" HTTP/1.0");
      client.println(" HTTP/1.1");
      client.println();
      Serial.println(oktext);
    } 
    else 
    {
      // if you didn't get a connection to the server
      Serial.println(errortext);
    }
    
    Serial.print("Receiving response...");

    bool test = true;
    
    while (test) 
    {
      // if there are incoming bytes available
      // from the server, read and check them
      if (client.available()) 
      {
        char c = client.read();
        response += c;

        // cast response obtained from string to char array
        char responsechar[response.length() + 1];
        response.toCharArray(responsechar, response.length() + 1);

        // if response includes a "200 OK" substring
        if (strstr(responsechar, "200 OK") != NULL) 
        {
          Serial.println(oktext);
          Serial.println("TEST COMPLETE!");
          test = false;
        }
      }

      // if the server's disconnected, stop the client:
      if (!client.connected()) 
      {
        Serial.println();
        Serial.println("disconnecting.");
        client.stop();
        test = false;
      }
    }
  }
}

/*
  Read input serial
 */
int readSerial(char result[]) 
{
  int i = 0;
  
  while (1) 
  {
    while (Serial.available() > 0) 
    {
      char inChar = Serial.read();
      
      if (inChar == '\n') 
      {
        result[i] = '\0';
        return 0;
      }
      
      if (inChar != '\r') 
      {
        result[i] = inChar;
        i++;
      }
    }
  }
}
