/*********************************************************************************************************************************
  GSMScanNetworks.ino

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

  Version: 1.3.0
  
  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.2.4    K Hoang     11/03/2021 Initial public release to add support to many boards / modules besides MKRGSM 1400 / SARA U201
  1.3.0    K Hoang     31/03/2021 Add ThingStream MQTTS support. Fix SMS receive bug.
 **********************************************************************************************************************************/
/*

  GSM Scan Networks

  This example prints out the IMEI number of the modem, then checks to see if it's connected to a carrier. If so, it prints the phone number
  associated with the card. Then it scans for nearby networks and prints out their signal strengths.

  Circuit:
   GSM module
   Antenna
   SIM card

  Created 8 Mar 2012
  by Tom Igoe, implemented by Javier Carazo
  Modified 4 Feb 2013
  by Scott Fitzgerald
*/

#include "defines.h"

// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = SECRET_PINNUMBER;

// BaudRate to communicate to GSM/GPRS modem. If be limit to max 115200 inside modem
unsigned long baudRateSerialGSM  = 115200;

// initialize the library instance
GSM gsmAccess;     // include a 'true' parameter to enable debugging
GSMScanner scannerNetworks;
GSMModem modemTest;

// Save data variables
String IMEI = "";

// serial monitor result messages
String errortext = "ERROR";

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial);

  delay(200);

  Serial.print(F("\nStarting GSMScanNetworks on ")); Serial.println(BOARD_NAME);
  Serial.println(GSM_GENERIC_VERSION);

#if ( defined(DEBUG_GSM_GENERIC_PORT) && (_GSM_GENERIC_LOGLEVEL_ > 4) )
  MODEM.debug(DEBUG_GSM_GENERIC_PORT);
#endif  

  Serial.println("GSM networks scanner");
  scannerNetworks.begin();

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

  // get modem parameters
  // IMEI, modem unique identifier
  Serial.print("Modem IMEI: ");
  IMEI = modemTest.getIMEI();
  IMEI.replace("\n", "");
  
  if (IMEI != NULL) 
  {
    Serial.println(IMEI);
  }
}

void loop() 
{
  // scan for existing networks, displays a list of networks
  Serial.println("Scanning available networks. May take some seconds.");
  Serial.println(scannerNetworks.readNetworks());

  // currently connected carrier
  Serial.print("Current carrier: ");
  Serial.println(scannerNetworks.getCurrentCarrier());

  // returns strength and ber
  // signal strength in 0-31 scale. 31 means power > 51dBm
  // BER is the Bit Error Rate. 0-7 scale. 99=not detectable
  Serial.print("Signal Strength: ");
  Serial.print(scannerNetworks.getSignalStrength());
  Serial.println(" [0-31]");
}
