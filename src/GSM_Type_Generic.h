/*********************************************************************************************************************************
  GSM_Type_Generic.h
  
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

#ifndef _GSM_TYPE_H_INCLUDED
#define _GSM_TYPE_H_INCLUDED

///////////////////////////////////////////////////////////////
//Global functions and declarations

#include <time.h>

#define GSM_UNUSED(var)     ((void) var)

extern "C" 
{
  char *strptime(const char *__restrict, const char *__restrict, struct tm *__restrict);
}

byte charToInt(byte input)
{
  if (input > '9') 
  {
    input = (input - 'A') + 10;
  } 
  else 
  {
    input = (input - '0');
  }
  
  return input;
}

////////////////////////////////////////////////////// 

String substringAfterSymbol(String str, char symbol)
{
  return( str.substring(str.indexOf(symbol) + 1) );
}

////////////////////////////////////////////////////// 

String substringBeforeSymbol(String str, char symbol)
{     
  return( str.substring(0, str.indexOf(symbol) ) );
}

////////////////////////////////////////////////////// 


#define GSM_NL "\r\n"

///////////////////////////////////////////////////////////////
// Originally from Modem_???_Generic.h

enum
{ 
  GSM_MODEM_START_ERROR = 0,
  GSM_MODEM_START_OK    = 1
};

enum 
{ 
  GSM_RESPONSE_TIMEOUT        = -1, 
  GSM_RESPONSE_IDLE           =  0, 
  GSM_RESPONSE_OK             =  1, 
  GSM_RESPONSE_ERROR          =  2, 
  GSM_RESPONSE_NO_CARRIER     =  3, 
};

///////////////////////////////////////////////////////////////
// Originally from GSM_Generic.h and GSM_Generic_Impl.hpp

enum GSM3_NetworkStatus_t 
{ 
  GSM_ERROR, 
  GSM_IDLE, 
  GSM_CONNECTING, 
  GSM_READY, 
  GPRS_READY, 
  GSM_TRANSPARENT_CONNECTED, 
  GSM_OFF 
};

enum 
{
  READY_STATE_CHECK_SIM,
  READY_STATE_WAIT_CHECK_SIM_RESPONSE,
  READY_STATE_UNLOCK_SIM,
  READY_STATE_WAIT_UNLOCK_SIM_RESPONSE,
  READY_STATE_SET_PREFERRED_MESSAGE_FORMAT,
  READY_STATE_WAIT_SET_PREFERRED_MESSAGE_FORMAT_RESPONSE,
  READY_STATE_SET_HEX_MODE,
  READY_STATE_WAIT_SET_HEX_MODE,
  READY_STATE_SET_AUTOMATIC_TIME_ZONE,
  READY_STATE_WAIT_SET_AUTOMATIC_TIME_ZONE_RESPONSE,
  READY_STATE_ENABLE_DTMF_DETECTION,
  READY_STATE_WAIT_ENABLE_DTMF_DETECTION_RESPONSE,
  READY_STATE_CHECK_REGISTRATION,
  READY_STATE_WAIT_CHECK_REGISTRATION_RESPONSE,
  READY_STATE_SET_REPORTING_CALL_STATUS,
  READY_STATE_WAIT_SET_REPORTING_CALL_STATUS,
  READY_STATE_DONE
};

// Originally from GPRS_Generic.h
enum 
{
  GPRS_PING_DEST_UNREACHABLE  = -1,
  GPRS_PING_TIMEOUT           = -2,
  GPRS_PING_UNKNOWN_HOST      = -3,
  GPRS_PING_ERROR             = -4
};

typedef struct 
{
  GSM3_NetworkStatus_t  _state;
  int                   _readyState;
  const char*           _pin;
  unsigned long         _timeout;
} GSM_Data;

///////////////////////////////////////////////////////////////
// Originally from GPRS_Generic_Impl.hpp
enum
{
  GPRS_STATE_IDLE,
  GPRS_STATE_ATTACH,
  GPRS_STATE_WAIT_ATTACH_RESPONSE,
  GPRS_STATE_SET_APN,
  GPRS_STATE_WAIT_SET_APN_RESPONSE,
  GPRS_STATE_SET_AUTH_MODE,
  GPRS_STATE_WAIT_SET_AUTH_MODE_RESPONSE,
  GPRS_STATE_SET_USERNAME,
  GPRS_STATE_WAIT_SET_USERNAME_RESPONSE,
  GPRS_STATE_SET_PASSWORD,
  GPRS_STATE_WAIT_SET_PASSWORD_RESPONSE,
  GPRS_STATE_SET_DYNAMIC_IP,
  GPRS_STATE_WAIT_SET_DYNAMIC_IP_RESPONSE,
  GPRS_STATE_ACTIVATE_IP,
  GPRS_STATE_WAIT_ACTIVATE_IP_RESPONSE,
  GPRS_STATE_CHECK_PROFILE_STATUS,
  GPRS_STATE_WAIT_CHECK_PROFILE_STATUS_RESPONSE,

  GPRS_STATE_DEACTIVATE_IP,
  GPRS_STATE_WAIT_DEACTIVATE_IP_RESPONSE,
  GPRS_STATE_DEATTACH,
  GPRS_STATE_WAIT_DEATTACH_RESPONSE,
  // New for SIM800/SIM900
  GPRS_STATE_DEFINE_PDP_CONTEXT,
  GPRS_STATE_WAIT_DEFINE_PDP_CONTEXT_RESPONSE,
  GPRS_STATE_ACTIVATE_PDP_CONTEXT,
  GPRS_STATE_WAIT_ACTIVATE_PDP_CONTEXT_RESPONSE,
  GPRS_STATE_OPEN_GPRS_BEARER_CONTEXT,
  GPRS_STATE_WAIT_OPEN_GPRS_BEARER_CONTEXT_RESPONSE,
  GPRS_STATE_QUERY_GPRS_BEARER_CONTEXT,
  GPRS_STATE_WAIT_QUERY_GPRS_BEARER_CONTEXT_RESPONSE,
  GPRS_STATE_ATTACH_GPRS,
  GPRS_STATE_WAIT_ATTACH_GPRS_RESPONSE,
  GPRS_STATE_SET_MULTI_IP,
  GPRS_STATE_WAIT_SET_MULTI_IP_RESPONSE,
  GPRS_STATE_QUICK_SEND,
  GPRS_STATE_WAIT_QUICK_SEND_RESPONSE,
  GPRS_STATE_START_TASK,
  GPRS_STATE_WAIT_START_TASK_RESPONSE,
  GPRS_STATE_WIRELESS_UP,
  GPRS_STATE_WAIT_WIRELESS_UP_RESPONSE,
  GPRS_STATE_GET_IP,
  GPRS_STATE_WAIT_GET_IP_RESPONSE,
  GPRS_STATE_CONFIG_DNS,
  GPRS_STATE_WAIT_CONFIG_DNS_RESPONSE
  ////////////////
};

///////////////////////////////////////////////////////////////
// Originally from GSMClient_Generic_Impl.hpp
enum
{
  CLIENT_STATE_IDLE,
  CLIENT_STATE_CREATE_SOCKET,
  CLIENT_STATE_WAIT_CREATE_SOCKET_RESPONSE,
  CLIENT_STATE_ENABLE_SSL,
  CLIENT_STATE_WAIT_ENABLE_SSL_RESPONSE,
  CLIENT_STATE_MANAGE_SSL_PROFILE,
  CLIENT_STATE_WAIT_MANAGE_SSL_PROFILE_RESPONSE,
  CLIENT_STATE_CONNECT,
  CLIENT_STATE_WAIT_CONNECT_RESPONSE,
  CLIENT_STATE_CLOSE_SOCKET,
  CLIENT_STATE_WAIT_CLOSE_SOCKET
};

// Originally from GSMServer_Generic_Impl.hpp
enum
{
  SERVER_STATE_IDLE,
  SERVER_STATE_CREATE_SOCKET,
  SERVER_STATE_WAIT_CREATE_SOCKET_RESPONSE,
  SERVER_STATE_LISTEN,
  SERVER_STATE_WAIT_LISTEN_RESPONSE,
  SERVER_STATE_CLOSE_SOCKET,
  SERVER_STATE_WAIT_CLOSE_SOCKET
};

///////////////////////////////////////////////////////////////
// Originally from GSMBand_Generic.h
#define NUMBEROFBANDS       9

#define GSM_MODE_UNDEFINED            "UNDEFINED"
#define GSM_MODE_EGSM                 "EGSM_MODE"
#define GSM_MODE_DCS                  "DCS_MODE"
#define GSM_MODE_PCS                  "PCS_MODE"
#define GSM_MODE_EGSM_DCS             "EGSM_DCS_MODE"
#define GSM_MODE_GSM850_PCS           "GSM850_PCS_MODE"
#define GSM_MODE_GSM850_EGSM_DCS_PCS  "GSM850_EGSM_DCS_PCS_MODE"
#define GSM_MODE_UMTS                 "UMTS_MODE"
#define GSM_MODE_GSM850_EGSM_PCS_UMTS "GSM850_EGSM_PCS_UMTS_MODE"

enum GSM3GSMBand 
{
  UNDEFINED, 
  EGSM_MODE, 
  DCS_MODE, 
  PCS_MODE, 
  EGSM_DCS_MODE, 
  GSM850_PCS_MODE, 
  GSM850_EGSM_DCS_PCS_MODE, 
  GSM_UMTS_MODE, 
  GSM_GSM850_EGSM_PCS_UMTS_MODE
};

///////////////////////////////////////////////////////////////
// Originally from GSMBand_Generic_Impl.hpp

/* Used in URAT for set the access technology, the first value indicate
  the technology used (0 GSM/GPRS/eGSM (single mode) , 1 GSM/GPRS/eGSM and UMTS(dual mode))
  the second parameter set, if more than one, which technology's bands should be preferred(0 GSM/GPRS/eGSM, 2 UTRAN)*/
  
// Page 106, u-blox AT command
#define GSM_GPRS_SINGLE_MODE    "0"
#define GSM_UMTS_DUAL_MODE      "1"
#define UMTS_SINGLE_MODE        "2"
#define LTE_SINGLE_MODE         "3"
#define GSM_UMTS_LTE_TRI_MODE   "4"
#define GSM_LTE_DUAL_MODE       "5"
#define UMTS_LTE_DUAL_MODE      "6"

#define GSM_BANDS               "1,0"
#define UMTS_BANDS              "1,2"

///////////////////////////////////////////////////////////////
// Originally from GSM_SMS_Generic.h

enum 
{
  SMS_STATE_IDLE,
  SMS_STATE_LIST_MESSAGES,
  SMS_STATE_WAIT_LIST_MESSAGES_RESPONSE
};

typedef struct
{
  bool      synch;
  int       state;
  int       smsDataIndex;
  int       smsDataEndIndex;
  bool      smsTxActive;
} GSM_SMS_Data;

// Originally from GSM_SSLClient_Generic_Impl.hpp

enum
{
  SSL_CLIENT_STATE_LOAD_ROOT_CERT,
  SSL_CLIENT_STATE_WAIT_LOAD_ROOT_CERT_RESPONSE,
  SSL_CLIENT_STATE_WAIT_DELETE_ROOT_CERT_RESPONSE
};

///////////////////////////////////////////////////////////////
// Originally from GSM_VoiceCall_Generic_Impl.hpp

enum GSM3_voiceCall_st 
{ 
  IDLE_CALL, 
  CALLING, 
  RECEIVINGCALL, 
  TALKING
};

///////////////////////////////////////////////////////////////
// Originally from GSMSocketBuffer_Generic.h

#define GSM_SOCKET_BUFFER_SIZE      512
#define GSM_SOCKET_BUFFER_NUMBER    7

typedef struct 
{
  uint8_t* data;
  uint8_t* head;
  int length;
} SocketBufferList;
    
///////////////////////////////////////////////////////////////
// SIM PIN related definitions

enum
{
  SIM_PIN_UNLOCKED    =  0, 
  SIM_PIN_NEED_PIN    =  1,
  SIM_PIN_NEED_PUK    = -1,
  SIM_PIN_ERROR       = -2,
  SIM_PIN_WRONG_PIN   = -3
};


enum
{ 
  SIM_PIN_WRITE_SUCCESS   =  1, 
  SIM_PIN_WRITE_ERROR     = -1,
  SIM_PIN_WRITE_WRONG_PIN = -2
};

/*
  Network registration status +CREG
  • 0: not registered, the MT is not currently searching a new operator to register to
  • 1: registered, home network
  • 2: not registered, but the MT is currently searching a new operator to register to
  • 3: registration denied
  • 4: unknown (e.g. out of GERAN/UTRAN/E-UTRAN coverage)
  • 5: registered, roaming
  • 6: registered for "SMS only", home network (applicable only when <AcTStatus>
  indicates E-UTRAN)
  • 7: registered for "SMS only", roaming (applicable only when <AcTStatus> indicates
  E-UTRAN)
  • 8: attached for emergency bearer services only (see 3GPP TS 24.008 [65] and 3GPP
  TS 24.301 [100] that specify the condition when the MS is considered as attached
  for emergency bearer services)
  • 9: registered for "CSFB not preferred", home network (applicable only when
  <AcTStatus> indicates E-UTRAN)
  • 10: registered for "CSFB not preferred", roaming (applicable only when <AcTStatus>
  indicates E-UTRAN)
*/

enum
{ 
  GSM_NETWORK_REGISTRATION_ERROR              = -1,
  GSM_NETWORK_NOT_REGISTERED_NOT_SEARCHING    =  0, 
  GSM_NETWORK_HOME                            =  1,
  GSM_NETWORK_NOT_REGISTERED_SEARCHING        =  2,
  GSM_NETWORK_REGISTRATION_DENIED             =  3,
  GSM_NETWORK_UNKNOWN                         =  4, 
  GSM_NETWORK_ROAMING                         =  5,
  GSM_NETWORK_HOME_SMS_ONLY                   =  6,
  GSM_NETWORK_ROAMING_SMS_ONLY                =  7,
  GSM_NETWORK_EMERGENCY_ONLY                  =  8, 
  GSM_NETWORK_HOME_CSFB_NOT_PREFERRED         =  9,
  GSM_NETWORK_ROAMING_CSFB_NOT_PREFERRED      =  10
};

enum
{ 
  GSM_NETWORK_CHECK_IS_ERROR                  = -1,
  GSM_NETWORK_CHECK_IS_HOME                   =  0,
  GSM_NETWORK_CHECK_IS_ROAMING                =  1,
  GSM_NETWORK_CHECK_IS_EMERGENCY_ONLY         =  2,
};
    
#endif    // _GSM_TYPE_H_INCLUDED
