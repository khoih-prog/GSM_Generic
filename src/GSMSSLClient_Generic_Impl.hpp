/*********************************************************************************************************************************
  GSMSSLClient_Generic_Impl.hpp
  
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

#ifndef _GSM_SSL_CLIENT_IMPL_HPP
#define _GSM_SSL_CLIENT_IMPL_HPP

//#include "GSMSSLClient_Generic.h"

bool GSMSSLClient::_rootCertsLoaded = false;

GSMSSLClient::GSMSSLClient(bool synch) :
  GSMClient(synch),
  _gsmRoots(GSM_ROOT_CERTS),
  _sizeRoot(GSM_NUM_ROOT_CERTS)
{
}

GSMSSLClient::~GSMSSLClient()
{
}

int GSMSSLClient::ready()
{
  if (_rootCertsLoaded)
  {
    // root certs loaded already, continue to regular GSMClient
    return GSMClient::ready();
  }

  int ready = MODEM.ready();

  if (ready == 0)
  {
    // a command is still running
    return 0;
  }

  switch (_state)
  {
    case SSL_CLIENT_STATE_LOAD_ROOT_CERT:
      {
        if (_gsmRoots[_certIndex].size)
        {
          // load the next root cert
          // SSL/TLS certificates and private keys manager +USECMNG
          // 0: import a certificate or a private key (data provided by the stream of byte)
          // 0: from trusted root CA (certificate authority) certificate
          MODEM.loadRootCert(_gsmRoots[_certIndex].name, _gsmRoots[_certIndex].size);

          if (MODEM.waitForPrompt() != 1)
          {
            // failure
            ready = -1;
          }
          else
          {
            // send the cert contents
            MODEM.write(_gsmRoots[_certIndex].data, _gsmRoots[_certIndex].size);

            _state = SSL_CLIENT_STATE_WAIT_LOAD_ROOT_CERT_RESPONSE;
            ready = 0;
          }
        }
        else
        {
          // remove the next root cert name
          // 2: remove an imported certificate or private key
          // 0: from trusted root CA (certificate authority) certificate 
          MODEM.removeRootCert(_gsmRoots[_certIndex].name);

          _state = SSL_CLIENT_STATE_WAIT_DELETE_ROOT_CERT_RESPONSE;
          ready = 0;
        }

        break;
      }

    case SSL_CLIENT_STATE_WAIT_LOAD_ROOT_CERT_RESPONSE:
      {
        if (ready > 1)
        {
          // error
        }
        else
        {
          _certIndex++;

          if (_certIndex == _sizeRoot)
          {
            // all certs loaded
            _rootCertsLoaded = true;
          }
          else
          {
            // load next
            _state = SSL_CLIENT_STATE_LOAD_ROOT_CERT;
          }

          ready = 0;
        }

        break;
      }

    case SSL_CLIENT_STATE_WAIT_DELETE_ROOT_CERT_RESPONSE:
      {
        // ignore ready response, root cert might not exist
        _certIndex++;

        if (_certIndex == GSM_NUM_ROOT_CERTS) 
        {
          // all certs loaded
          _rootCertsLoaded = true;
        } 
        else 
        {
          // load next
          _state = SSL_CLIENT_STATE_LOAD_ROOT_CERT;
        }

        ready = 0;
        
        break;
      }
  }

  return ready;
}

int GSMSSLClient::connect(IPAddress ip, uint16_t port, bool loadRootCert)
{
  if (loadRootCert)
  {
    // Root Certs not yet loaded and saved. Load now just once
    _certIndex = 0;
    _state = SSL_CLIENT_STATE_LOAD_ROOT_CERT;
  }
  else
  {
    // Root Certs already loaded and saved last time. 
    _certIndex = _sizeRoot;
    _state = SSL_CLIENT_STATE_LOAD_ROOT_CERT;
    
    // all certs loaded
    _rootCertsLoaded = true;
  }

  return connectSSL(ip, port);
}

int GSMSSLClient::connect(const char* host, uint16_t port, bool loadRootCert)
{
  if (loadRootCert)
  {
    // Root Certs not yet loaded and saved. Load now just once
    _certIndex = 0;
    _state = SSL_CLIENT_STATE_LOAD_ROOT_CERT;
  }
  else
  {
    // Root Certs already loaded and saved last time. 
    _certIndex = _sizeRoot;
    _state = SSL_CLIENT_STATE_LOAD_ROOT_CERT;
    
    // all certs loaded
    _rootCertsLoaded = true;
  }

  return connectSSL(host, port);
}

void GSMSSLClient::setSignedCertificate(const uint8_t* cert, const char* name, size_t size) 
{
  GSM_LOGDEBUG3(F("GSMSSLClient::setSignedCertificate: name = "), name, F(", size = "), size);
  
  // SSL/TLS certificates and private keys manager +USECMNG
  MODEM.setSignedCertificate(name, size);
  
  MODEM.waitForResponse(1000);

  MODEM.write(cert, size);
  MODEM.waitForResponse(1000);
}

void GSMSSLClient::setPrivateKey(const uint8_t* key, const char* name, size_t size) 
{
  GSM_LOGDEBUG3(F("GSMSSLClient::setPrivateKey: name = "), name, F(", size = "), size);
  
  // SSL/TLS certificates and private keys manager +USECMNG
  MODEM.setPrivateKey(name, size);
  
  MODEM.waitForResponse(1000);
  MODEM.write(key, size);
  MODEM.waitForResponse(1000);
}

void GSMSSLClient::setTrustedRoot(const char* name) 
{
  GSM_LOGDEBUG1(F("GSMSSLClient::setTrustedRoot: name = "), name);
  
  //SSL/TLS/DTLS security layer profile manager +USECPRF
  MODEM.setTrustedRoot(name);
  
  MODEM.waitForResponse(100);
}

void GSMSSLClient::useSignedCertificate(const char* name) 
{
  GSM_LOGDEBUG1(F("GSMSSLClient::useSignedCertificate: name = "), name);
  
  //SSL/TLS/DTLS security layer profile manager +USECPRF
  MODEM.useSignedCertificate(name);
  
  MODEM.waitForResponse(100);
}

void GSMSSLClient::usePrivateKey(const char* name) 
{
  GSM_LOGDEBUG1(F("GSMSSLClient::usePrivateKey: name = "), name);
  
  //SSL/TLS/DTLS security layer profile manager +USECPRF
  MODEM.usePrivateKey(name);
  
  MODEM.waitForResponse(100);
}

void GSMSSLClient::eraseTrustedRoot() 
{
  GSM_LOGDEBUG(F("GSMSSLClient::eraseTrustedRoot"));
  
  for (int i = 0; i < _sizeRoot; i++) 
  {
    GSM_LOGDEBUG3(F("GSMSSLClient::eraseTrustedRoot: i = "), i, F(", name = "), _gsmRoots[i].name);
    
    // SSL/TLS certificates and private keys manager +USECMNG
    MODEM.eraseTrustedRoot(_gsmRoots[i].name);
    
    MODEM.waitForResponse(100);
  }
}

void GSMSSLClient::setUserRoots(const GSMRootCert * userRoots, size_t size) 
{
  _gsmRoots = userRoots;
  _sizeRoot = size;
}

#endif    // _GSM_SSL_CLIENT_IMPL_HPP
