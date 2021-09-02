/*********************************************************************************************************************************
  GSMHttpUtils_Generic.h
  
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

#ifndef _GSM_HTTP_UTILS_H_INCLUDED
#define _GSM_HTTP_UTILS_H_INCLUDED

#include "GSM_Generic_Debug.h"

#include "GSMClient_Generic.h"
#include "utility/GSMRootCerts_Generic.h"

class GSMHttpUtils: public GSMClient
{
  public:
    GSMHttpUtils();
    virtual ~GSMHttpUtils();

    virtual void setSignedCertificate(const uint8_t* cert, const char* name, size_t size);
    virtual void setPrivateKey(const uint8_t* key, const char* name, size_t size);
    virtual void useSignedCertificate(const char* name);
    virtual void usePrivateKey(const char* name);
    virtual void setTrustedRoot(const char* name);
    virtual void setUserRoots(const GSMRootCert * userRoots, size_t size);
    virtual void eraseTrustedRoot();
    virtual void eraseAllCertificates();
    virtual void eraseCert(const char* name, int type);
    virtual void handleUrc(const String& urc);
    virtual void enableSSL();
    virtual void disableSSL();
    virtual void configServer(const char* url, int httpport);
    virtual void head(const char* path, const char* filename);
    virtual void get(const char* path, const char* filename);
    virtual void del(const char* path, const char* filename);
    virtual void put(const char* path, const char* filename);
    virtual void post(const char* path, const char* filename);
    virtual bool responseStatus();
    
  private:
    void removeCertForType(String certname, int type);

  private:
    static bool         _rootCertsLoaded;
    int                 _certIndex;
    const GSMRootCert * _gsmRoots;
    int                 _sizeRoot;
    bool                _httpresp;
    bool                _ssl;

};

#include "GSMHttpUtils_Generic_Impl.hpp"

#endif    // _GSM_HTTP_UTILS_H_INCLUDED
