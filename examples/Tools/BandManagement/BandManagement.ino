/*********************************************************************************************************************************
  BandManagement.ino

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
  Band Management

  This sketch, for the GSM module, checks the band currently configured in the modem and allows you to change it.

  Please check http://www.worldtimezone.com/gsm.html

  Usual configurations:
  Europe, Africa, Middle East: E-GSM(900)+DCS(1800)
  USA, Canada, South America : GSM(850)+PCS(1900)
  Mexico: PCS(1900)
  Brazil: GSM(850)+E-GSM(900)+DCS(1800)+PCS(1900)


  Circuit:
   GSM module
   Antenna

  created 12 June 2012
  by Javier Zorzano, Scott Fitzgerald
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

// BaudRate to communicate to GSM/GPRS modem. If be limit to max 115200 inside modem
unsigned long baudRateSerialGSM  = 115200;

// libraries
#include <GSM_Generic_Main.h>

// initialize the library instance
GSMBand band;

// This function offers the user different options
// through the Serial interface
// The user selects one
String askUser() 
{
  String newBand;
  
  Serial.println("Select band:");
  // Print the different options
  Serial.println("1 : E-GSM(900)");
  Serial.println("2 : DCS(1800)");
  Serial.println("3 : PCS(1900)");
  Serial.println("4 : E-GSM(900)+DCS(1800) ex: Europe");
  Serial.println("5 : GSM(850)+PCS(1900) Ex: USA, South Am.");
  Serial.println("6 : GSM800(800)+GSM(850)+E-GSM(900)+PCS(1900)");
  Serial.println("7 : UMTS(2100)");
  Serial.println("8 : GSM(850)+E-GSM(900)+PCS(1900)+UMTS(2100)");

  // Empty the incoming buffer
  while (Serial.available()) 
  {
    Serial.read();
  }

  // Wait for an answer, just look at the first character
  while (!Serial.available());
  
  char c = Serial.read();
  
  if (c == '1') 
  {
    newBand = GSM_MODE_EGSM;
  } 
  else if (c == '2') 
  {
    newBand = GSM_MODE_DCS;
  } 
  else if (c == '3') 
  {
    newBand = GSM_MODE_PCS;
  } 
  else if (c == '4') 
  {
    newBand = GSM_MODE_EGSM_DCS;
  } 
  else if (c == '5') 
  {
    newBand = GSM_MODE_GSM850_PCS;
  } 
  else if (c == '6') 
  {
    newBand = GSM_MODE_GSM850_EGSM_DCS_PCS;
  } 
  else if (c == '7') 
  {
    newBand = GSM_MODE_UMTS;
  } 
  else if (c == '8') 
  {
    newBand = GSM_MODE_GSM850_EGSM_PCS_UMTS;
  } 
  else 
  {
    newBand = "GSM_MODE_UNDEFINED";
  }

  return newBand;
}

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial);

  delay(200);

  Serial.print(F("\nStarting BandManagement on ")); Serial.println(BOARD_NAME);
  Serial.println(GSM_GENERIC_VERSION);

#if ( defined(DEBUG_GSM_GENERIC_PORT) && (_GSM_GENERIC_LOGLEVEL_ > 5) )
  MODEM.debug(DEBUG_GSM_GENERIC_PORT);
#endif  

  // Beginning the band manager restarts the modem
  Serial.println("Restarting modem...");
  band.begin(baudRateSerialGSM);
  Serial.println("Modem restarted.");
};


void loop() 
{
  // Get current band
  String bandName = band.getBand(); // Get and print band name
  Serial.print("Current band:");
  Serial.println(bandName);
  Serial.println("Want to change the band you’re on?");
  
  String newBandName;
  newBandName = askUser();
  
  // Tell the user what we are about to do…
  Serial.print("\nConfiguring band ");
  Serial.println(newBandName);
  
  // Change the band
  bool operationSuccess;
  operationSuccess = band.setBand(newBandName);
  
  // Tell the user if the operation was OK
  if (operationSuccess) 
  {
    Serial.println("Success");
  } 
  else 
  {
    Serial.println("Error while changing band");
  }

  if (operationSuccess) 
  {
    while (true);
  }
}
