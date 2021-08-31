/*********************************************************************************************************************************
  MakeVoiceCall.ino

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
  Make Voice Call

  This sketch, for the GSM module, puts a voice call to a remote phone number that you enter through the serial monitor.
  To make it work, open the serial monitor, and when you see the READY message, type a phone number. Make sure the 
  serial monitor is set to send a just newline when you press return.

  Circuit:
   GSM Module
   Antenna
   SIM card that can send voice calls


  created Mar 2012
  by Javier Zorzano
*/

#include "defines.h"

// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = SECRET_PINNUMBER;

// BaudRate to communicate to GSM/GPRS modem. If be limit to max 115200 inside modem
unsigned long baudRateSerialGSM  = 115200;

// initialize the library instance
GSM gsmAccess; // include a 'true' parameter for debug enabled
GSMVoiceCall vcs;

String remoteNumber = "";  // the number you will call
char charbuffer[20];

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial);

  delay(200);

  Serial.print(F("\nStarting MakeVoiceCall on ")); Serial.println(BOARD_NAME);
  Serial.println(GSM_GENERIC_VERSION);

#if ( defined(DEBUG_GSM_GENERIC_PORT) && (_GSM_GENERIC_LOGLEVEL_ > 4) )
  MODEM.debug(DEBUG_GSM_GENERIC_PORT);
#endif  

  // connection state
  bool connected = false;

  // Start GSM shield
  // If your SIM has PIN, pass it as a parameter of begin() in quotes
  while (!connected) 
  {
    if (gsmAccess.begin(baudRateSerialGSM, PINNUMBER) == GSM_READY) 
    {
      connected = true;
    } 
    else 
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("GSM initialized.");
  Serial.println("Enter phone number to call.");
}

void loop() 
{
  // add any incoming characters to the String:
  while (Serial.available() > 0) 
  {
    char inChar = Serial.read();
    
    // if it's a newline, that means you should make the call:
    if (inChar == '\n') 
    {
      // make sure the phone number is not too long:
      if (remoteNumber.length() < 20) 
      {
        // let the user know you're calling:
        Serial.print("Calling to : ");
        Serial.println(remoteNumber);
        Serial.println();

        // Call the remote number
        remoteNumber.toCharArray(charbuffer, 20);

        // Check if the receiving end has picked up the call
        if (vcs.voiceCall(charbuffer)) 
        {
          Serial.println("Call Established. Enter line to end");
          // Wait for some input from the line
          while (Serial.read() != '\n' && (vcs.getvoiceCallStatus() == TALKING));
          // And hang up
          vcs.hangCall();
        }
        
        Serial.println("Call Finished");
        remoteNumber = "";
        Serial.println("Enter phone number to call.");
      } 
      else 
      {
        Serial.println("That's too long for a phone number. I'm forgetting it");
        remoteNumber = "";
      }
    } 
    else 
    {
      // add the latest character to the message to send:
      if (inChar != '\r') 
      {
        remoteNumber += inChar;
      }
    }
  }
}
