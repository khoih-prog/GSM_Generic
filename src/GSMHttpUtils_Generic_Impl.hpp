/*********************************************************************************************************************************
  GSMHttpUtils_Generic_Impl.hpp
  
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

#ifndef _GSM_HTTP_UTILS_HPP
#define _GSM_HTTP_UTILS_HPP

//#include "GSMHttpUtils_Generic.h"

#define TRUST_ROOT_TYPE       "CA,\""
#define CLIENT_CERT_TYPE      "CC,\""
#define CLIENT_KEY_TYPE       "PK,\""

bool GSMHttpUtils::_rootCertsLoaded = false;

GSMHttpUtils::GSMHttpUtils() :
  _gsmRoots(GSM_ROOT_CERTS),
  _sizeRoot(GSM_NUM_ROOT_CERTS),
  _httpresp(false),
  _ssl(false)
{
  MODEM.addUrcHandler(this);
}

GSMHttpUtils::~GSMHttpUtils()
{
  MODEM.removeUrcHandler(this);
}


void GSMHttpUtils::setSignedCertificate(const uint8_t* cert, const char* name, size_t size) 
{
  // SSL/TLS certificates and private keys manager +USECMNG
  MODEM.setSignedCertificate(name, size);
  
  MODEM.waitForResponse(1000);

  MODEM.write(cert, size);
  MODEM.waitForResponse(1000);
}

void GSMHttpUtils::setPrivateKey(const uint8_t* key, const char*name, size_t size) 
{
  // SSL/TLS certificates and private keys manager +USECMNG
  MODEM.setPrivateKey(name, size);
  
  MODEM.waitForResponse(1000);
  MODEM.write(key, size);
  MODEM.waitForResponse(1000);
}

void GSMHttpUtils::setTrustedRoot(const char* name) 
{
  //SSL/TLS/DTLS security layer profile manager +USECPRF
  MODEM.setTrustedRoot(name);
  
  MODEM.waitForResponse(100);
}

void GSMHttpUtils::useSignedCertificate(const char* name) 
{
  //SSL/TLS/DTLS security layer profile manager +USECPRF
  MODEM.useSignedCertificate(name);
  
  MODEM.waitForResponse(100);
}

void GSMHttpUtils::usePrivateKey(const char* name) 
{
  //SSL/TLS/DTLS security layer profile manager +USECPRF
  MODEM.usePrivateKey(name);
  
  MODEM.waitForResponse(100);
}

void GSMHttpUtils::eraseTrustedRoot() 
{
  for (int i = 0; i < _sizeRoot; i++) 
  {
    eraseCert(_gsmRoots[i].name, 0);
  }
}

void GSMHttpUtils::eraseAllCertificates() 
{
  for (int cert_type = 0; cert_type < 3; cert_type++) 
  {
    String response = "";
    
    // SSL/TLS certificates and private keys manager +USECMNG
    MODEM.eraseAllCertificates(cert_type);
    
    MODEM.waitForResponse(100, &response);
    
    int index = 0;
    bool done = true;
    
    if (response != "") 
    {
      while (done) 
      {
        int index_tmp = response.indexOf("\r\n", index);
        String certname = "";
        
        if (index_tmp > 0) 
        {
          certname = response.substring(index, index_tmp);
          index = index_tmp + 2;
        } 
        else 
        {
          certname = response.substring(index);
          done = false;
        }
        
        if (certname != "") 
        {
          removeCertForType(certname, cert_type);
        }
      }
    }
  }
}

void GSMHttpUtils::removeCertForType(String certname, int type) 
{
  int start_ind = -1;
  int last_ind = 0;
  
  switch (type) 
  {
    case 0:
      start_ind = certname.indexOf(TRUST_ROOT_TYPE) + sizeof(TRUST_ROOT_TYPE) - 1;
      break;
    case 1:
      start_ind = certname.indexOf(CLIENT_CERT_TYPE) + sizeof(CLIENT_CERT_TYPE) - 1;
      break;
    case 2:
      start_ind = certname.indexOf(CLIENT_KEY_TYPE) + sizeof(CLIENT_KEY_TYPE) - 1;
      break;
    default:
      break;
  }

  if (start_ind >= 0) 
  {
    last_ind = certname.indexOf("\"", start_ind);
    eraseCert(certname.substring(start_ind, last_ind).c_str(), type);
  }
}

void GSMHttpUtils::eraseCert(const char* name, int type) 
{
  // SSL/TLS certificates and private keys manager +USECMNG
  MODEM.eraseCert(name, type);
  
  MODEM.waitForResponse(100);
}

void GSMHttpUtils::setUserRoots(const GSMRootCert * userRoots, size_t size) 
{
  _gsmRoots = userRoots;
  _sizeRoot = size;
}

void GSMHttpUtils::handleUrc(const String& urc)
{
  _httpresp = MODEM.handleUrcFileUtils(urc);
}

void GSMHttpUtils::enableSSL() 
{
  // Sets the profile ID #1
  //SSL/TLS/DTLS security layer profile manager +USECPRF
  MODEM.enableSSL();
  
  MODEM.waitForResponse(100);
  _ssl = true;
}

void GSMHttpUtils::disableSSL() 
{
  if (_ssl) 
  {
    // Sets the profile ID #0
    //SSL/TLS/DTLS security layer profile manager +USECPRF
    MODEM.disableSSL();
    
    MODEM.waitForResponse(100);
  }
  
  _ssl = false;
}

void GSMHttpUtils::configServer(const char* url, int httpport) 
{
  // Reset the HTTP profile #0
  // HTTP control +UHTTP
  MODEM.configServerProfile0();
  
  MODEM.waitForResponse(100);

  // Sets HTTP server name
  // HTTP control +UHTTP
  MODEM.configServerNameProfile0(url);
  
  MODEM.waitForResponse(100);

  // Sets HTTP server port
  // HTTP control +UHTTP
  MODEM.configServerPortProfile0(httpport);
  
  MODEM.waitForResponse(100);

  // Sets HTTP secure option
  if (_ssl) 
  {
    // HTTP control +UHTTP
    MODEM.configServerSSLProfile0();
    
    MODEM.waitForResponse(100);
  }

  // DNS resolution of url
  // Resolve name / IP number through DNS +UDNSRN
  MODEM.resolveDNS(url);
  
  MODEM.waitForResponse(10000);
}

void GSMHttpUtils::head(const char* path, const char* filename) 
{
  // Makes a HEAD request and store the response in _file
  // HTTP command +UHTTPC
  MODEM.headRequest(path, filename);
  
  MODEM.waitForResponse(100);
}

void GSMHttpUtils::get(const char* path, const char* filename) 
{
  // Makes a GET request and store it in _file
  // HTTP command +UHTTPC
  MODEM.getRequest(path, filename);
  
  MODEM.waitForResponse(100);

}

void GSMHttpUtils::del(const char* path, const char* filename) 
{
  // make a DELETE request and store it in _file
  // HTTP command +UHTTPC
  MODEM.deleteRequest(path, filename);
  
  MODEM.waitForResponse(100);
}

void GSMHttpUtils::put(const char* path, const char* filename) 
{
  // make a PUT request and store it in _file
  MODEM.putRequest(path, filename);
  
  MODEM.waitForResponse(100);
}

void GSMHttpUtils::post(const char* path, const char* filename) 
{
  // make a POST request and store it in _file
  // HTTP command +UHTTPC
  MODEM.postRequest(path, filename);
  
  MODEM.waitForResponse(100);
}

bool GSMHttpUtils::responseStatus() 
{
  MODEM.poll();
  return _httpresp;
}

#endif    // _GSM_HTTP_UTILS_HPP
