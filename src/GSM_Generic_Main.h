/*********************************************************************************************************************************
  GSM_Generic_Main.h
  
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
 
  Version: 1.6.1
  
  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.2.4    K Hoang     11/03/2021 Initial public release to add support to many boards / modules besides MKRGSM 1400 / SARA U201
  1.3.0    K Hoang     31/03/2021 Add ThingStream MQTTS support. Fix SMS receive bug.
  1.3.1    K Hoang     25/04/2021 Fix bug making ESP32 reset repeatedly.
  1.4.0    K Hoang     28/06/2021 Add support to RP2040-based boards using Arduino mbed or Arduino-pico core
  1.5.0    K Hoang     14/08/2021 Add support to Adafruit nRF52 core v0.22.0+
  1.6.0    K Hoang     31/08/2021 Add support to Ublox LARA-R2xx LTE Cat 1 Modem
  1.6.1    K Hoang     31/08/2021 Fix GPRS issue for Ublox LARA-R2xx LTE Cat 1 Modem
 **********************************************************************************************************************************/

#pragma once

#ifndef _MKRGSM_H_INCLUDED
#define _MKRGSM_H_INCLUDED

#define GSM_GENERIC_VERSION     "GSM_Generic v1.6.1"

#include <stdarg.h>
#include <stdio.h>
#include "Arduino.h"

#include "GSM_Generic_Debug.h"

#include "GSM_Boards_Generic.h"

#if ( GSM_MODEM_UBLOX || TINY_GSM_MODEM_UBLOX )
  #warning Using GSM_MODEM_UBLOX
  #include "Modems/Modem_UBLOX_Generic.h"
#elif ( GSM_MODEM_SARAR4 || TINY_GSM_MODEM_SARAR4 )
  #warning Using GSM_MODEM_SARAR4
  #include "Modems/Modem_SaraR4_Generic.h"
#elif ( GSM_MODEM_LARAR2 || TINY_GSM_MODEM_LARAR2 )
  #warning Using GSM_MODEM_LARAR2
  #include "Modems/Modem_LaraR2_Generic.h"  
#elif ( GSM_MODEM_SIM800 || TINY_GSM_MODEM_SIM800 )
  //#error Using GSM_MODEM_SIM800 not supported yet
  #warning Using GSM_MODEM_SIM800
  #warning Using GSM_MODEM_SIM800
  #include "Modems/Modem_SIM800_Generic.h"  
#elif ( GSM_MODEM_SIM808 || TINY_GSM_MODEM_SIM808 )
  #error Using GSM_MODEM_SIM808 not supported yet
  #include "Modems/Modem_SIM808_Generic.h"  
#elif ( GSM_MODEM_SIM868 || TINY_GSM_MODEM_SIM868 )
  #error Using GSM_MODEM_SIM868 not supported yet
  #include "Modems/Modem_SIM868_Generic.h"  
#elif ( GSM_MODEM_SIM900 || TINY_GSM_MODEM_SIM900 )
  //#error Using GSM_MODEM_SIM900 not supported yet
  #warning Using GSM_MODEM_SIM900
  #include "Modems/Modem_SIM800_Generic.h"   
#elif ( GSM_MODEM_SIM5300 || TINY_GSM_MODEM_SIM5300 )
  #error Using GSM_MODEM_SIM5300 not supported yet
  #include "Modems/Modem_SIM5300_Generic.h"  
#elif ( GSM_MODEM_SIM5320 || TINY_GSM_MODEM_SIM5320 )
  #error Using GSM_MODEM_SIM5320 not supported yet
  #include "Modems/Modem_SIM5320_Generic.h"  
#elif ( GSM_MODEM_SIM5360 || TINY_GSM_MODEM_SIM5360 )
  #error Using GSM_MODEM_SIM5360 not supported yet
  #include "Modems/Modem_SIM5360_Generic.h"  
#elif ( GSM_MODEM_SIM7000 || TINY_GSM_MODEM_SIM7000 )
  #error Using GSM_MODEM_SIM7000 not supported yet
  #include "Modems/Modem_SIM7000_Generic.h"   
#elif ( GSM_MODEM_SIM7100 || TINY_GSM_MODEM_SIM7100 )
  #error Using GSM_MODEM_SIM7100 not supported yet
  #include "Modems/Modem_SIM7100_Generic.h"  
#elif ( GSM_MODEM_SIM7500 || TINY_GSM_MODEM_SIM7500 )
  #error Using GSM_MODEM_SIM7500 not supported yet
  #include "Modems/Modem_SIM7500_Generic.h"  
#elif ( GSM_MODEM_SIM7600 || TINY_GSM_MODEM_SIM7600 )
  #error Using GSM_MODEM_SIM7600 not supported yet
  #include "Modems/Modem_SIM7600_Generic.h"  
#elif ( GSM_MODEM_SIM7800 || TINY_GSM_MODEM_SIM7800 )
  #error Using GSM_MODEM_SIM7800 not supported yet
  #include "Modems/Modem_SIM7800_Generic.h"   
#elif ( GSM_MODEM_M95 || TINY_GSM_MODEM_M95 )
  #error Using GSM_MODEM_M95 not supported yet
  #include "Modems/Modem_M95_Generic.h"  
#elif ( GSM_MODEM_BG96 || TINY_GSM_MODEM_BG96 )
  #error Using GSM_MODEM_BG96 not supported yet
  #include "Modems/Modem_BG96_Generic.h"  
#elif ( GSM_MODEM_A6 || TINY_GSM_MODEM_A6 )
  #error Using GSM_MODEM_A6 not supported yet
  #include "Modems/Modem_A6_Generic.h"  
#elif ( GSM_MODEM_A7 || TINY_GSM_MODEM_A7 )
  #error Using GSM_MODEM_A7 not supported yet
  #include "Modems/Modem_A7_Generic.h"   
#elif ( GSM_MODEM_M590 || TINY_GSM_MODEM_M590 )
  #error Using GSM_MODEM_M590 not supported yet
  #include "Modems/Modem_M590_Generic.h"   
#elif ( GSM_MODEM_MC60 || TINY_GSM_MODEM_MC60 )
  #error Using GSM_MODEM_MC60 not supported yet
  #include "Modems/Modem_MC60_Generic.h"  
#elif ( GSM_MODEM_MC60E || TINY_GSM_MODEM_MC60E )
  #error Using GSM_MODEM_MC60E not supported yet
  #include "Modems/Modem_MC60E_Generic.h"  
#elif ( GSM_MODEM_XBEE || TINY_GSM_MODEM_XBEE )
  #error Using GSM_MODEM_XBEE not supported yet
  #include "Modems/Modem_XBEE_Generic.h"  
#elif ( GSM_MODEM_SEQUANS_MONARCH || TINY_GSM_MODEM_SEQUANS_MONARCH )
  #error Using GSM_MODEM_SEQUANS_MONARCH not supported yet
  #include "Modems/Modem_SequansMonarch_Generic.h" 
#else
  #error No GSM Modem selected yet
#endif  

////////////////////////////////////////

//ModemClass MODEM(SerialGSM, 921600, GSM_RESETN, GSM_DTR);
//ModemUrcHandler* ModemClass::_urcHandlers[MAX_URC_HANDLERS] = { NULL };
//Print* ModemClass::_debugPrint = NULL;

////////////////////////////////////////

//#include "Modem_Generic.h"

#include "GSM_Generic.h"
#include "GSMVoiceCall_Generic.h"
#include "GSM_SMS_Generic.h"
#include "GPRS_Generic.h"
#include "GSMClient_Generic.h"
#include "GSMServer_Generic.h"
#include "GSMModem_Generic.h"
#include "GSMScanner_Generic.h"
#include "GSMPIN_Generic.h"
#include "GSMBand_Generic.h"

#include "GSMSSLClient_Generic.h"
#include "GSMUdp_Generic.h"
#include "GSMLocation_Generic.h"

#include "GSMFileUtils_Generic.h"

#include "GSMHttpUtils_Generic.h"

#endif
