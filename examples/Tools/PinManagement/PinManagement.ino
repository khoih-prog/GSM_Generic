/*********************************************************************************************************************************
  PinManagement.ino

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

  This example enables you to change or remove the PIN number of a SIM card inserted into a GSM module.

  Circuit:
   GSM module
   Antenna
   SIM card

  Created 12 Jun 2012
  by David del Peral
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

// pin manager object
GSMPIN PINManager;

// save input in serial by user
String user_input = "";

// authenticated with PIN code
bool auth = false;

// serial monitor result messages
String oktext = "OK";
String errortext = "ERROR";

/*
  Read input serial
*/
String readSerial() 
{
  String text = "";
  
  while (1) 
  {
    while (Serial.available() > 0) 
    {
      char inChar = Serial.read();
      
      if (inChar == '\n') 
      {
        return text;
      }
      
      if (inChar != '\r') 
      {
        text += inChar;
      }
    }
  }
}

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial);

  delay(200);

  Serial.print(F("\nStarting PINManagement on ")); Serial.println(BOARD_NAME);
  Serial.println(GSM_GENERIC_VERSION);

#if ( defined(DEBUG_GSM_GENERIC_PORT) && (_GSM_GENERIC_LOGLEVEL_ > 4) )
  MODEM.debug(DEBUG_GSM_GENERIC_PORT);
#endif  
  
  PINManager.begin(baudRateSerialGSM);

  // check if the SIM have pin lock
  while (!auth) 
  {
    int pin_query = PINManager.isPIN();
    
    if (pin_query == SIM_PIN_NEED_PIN) 
    {
      // if SIM is locked, enter PIN code
      Serial.print("Enter PIN code: ");
      user_input = readSerial();
      
      // check PIN code
      if (PINManager.checkPIN(user_input) == SIM_PIN_UNLOCKED) 
      {
        auth = true;
        PINManager.setPINUsed(true);
        Serial.println(oktext);
      } 
      else 
      {
        // if PIN code was incorrected
        Serial.println("Incorrect PIN. Remember that you have 3 opportunities.");
      }
    } 
    else if (pin_query == SIM_PIN_NEED_PUK) 
    {
      // PIN code is locked, user must enter PUK code
      Serial.println("PIN locked. Enter PUK code: ");
      String puk = readSerial();
      Serial.print("Now, enter a new PIN code: ");
      user_input = readSerial();
      
      // check PUK code
      if (PINManager.checkPUK(puk, user_input) == SIM_PIN_UNLOCKED) 
      {
        auth = true;
        PINManager.setPINUsed(true);
        Serial.println(oktext);
      } 
      else 
      {
        // if PUK or the new PIN are incorrect
        Serial.println("Incorrect PUK or invalid new PIN. Try again!.");
      }
    } 
    else if (pin_query == SIM_PIN_ERROR) 
    {
      // the worst case, PIN and PUK are locked
      Serial.println("PIN and PUK locked. Use PIN2/PUK2 in a mobile phone.");
      while (true);
    } 
    else 
    {
      // SIM does not requires authetication
      Serial.println("No pin necessary.");
      auth = true;
    }
  }

  // start GSM shield
  Serial.print("Checking register in GSM network...");
  
  if (PINManager.checkReg() == GSM_NETWORK_CHECK_IS_HOME) 
  {
    Serial.println(oktext);
  }
  // if you are connect by roaming
  else if (PINManager.checkReg() == GSM_NETWORK_CHECK_IS_ROAMING) 
  {
    Serial.println("ROAMING " + oktext);
  } 
  else 
  {
    // error connection
    Serial.println(errortext);
    while (true);
  }
}

void loop() 
{
  // Function loop implements pin management user menu
  // Only if you SIM use pin lock, you can change PIN code
  // user_op variables save user option

  Serial.println("Choose an option:\n1 - On/Off PIN.");
  
  if (PINManager.getPINUsed()) 
  {
    Serial.println("2 - Change PIN.");
  }
  
  String user_op = readSerial();
  
  if (user_op == "1") 
  {
    Serial.println("Enter your PIN code:");
    user_input = readSerial();
    // activate/deactivate PIN lock
    PINManager.switchPIN(user_input);
  } 
  else if (user_op == "2" && PINManager.getPINUsed()) 
  {
    Serial.println("Enter your actual PIN code:");
    String oldPIN = readSerial();
    Serial.println("Now, enter your new PIN code:");
    String newPIN = readSerial();
    // change PIN
    PINManager.changePIN(oldPIN, newPIN);
  } 
  else 
  {
    Serial.println("Incorrect option. Try again!.");
  }
  
  delay(1000);
}
