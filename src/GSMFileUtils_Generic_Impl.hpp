/*********************************************************************************************************************************
  GSMFileUtils_Generic_Impl.hpp
  
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

#ifndef _GSM_FILE_UTILS_HPP
#define _GSM_FILE_UTILS_HPP

//#include "GSMFileUtils_Generic.h"


GSMFileUtils::GSMFileUtils(bool debug)
  : _count(0)
  , _files("")
  , _debug(debug)
{
}

bool GSMFileUtils::begin(unsigned long baud, const bool restart)
{
  int status;

  if (restart)
    MODEM.begin(baud);

  if (_debug) 
  {
    MODEM.debug();
    
    // Report mobile termination error +CMEE
    MODEM.setReportError();
    
    MODEM.waitForResponse();
  }

  for (unsigned long start = millis(); (millis() - start) < 10000;) 
  {
    status = _getFileList();
    
    if (status == 1) 
    {
      _countFiles();
      return true;
    }
    
    MODEM.poll();
  }
  
  return false;
}

int GSMFileUtils::_getFileList()
{
  String response;

  // List files information +ULSTFILE. List of files stored into the FS
  MODEM.listFile();
  
  int status = MODEM.waitForResponse(5000, &response);
  
  if (!response.length())
    return -1;

  if (status == GSM_RESPONSE_OK) 
  {
    String list = response.substring(11);
    list.trim();
    _files = list;
  }

  return status;
}

void GSMFileUtils::_countFiles()
{
  String list = _files;
  size_t len = 0;

  if (list.length() > 0) 
  {
    for (int index = list.indexOf(','); index != -1; index = list.indexOf(',')) 
    {
      list.remove(0, index + 1);
      ++len;
    }
    
    ++len;
  }
  
  _count = len;
}

size_t GSMFileUtils::listFiles(String files[]) const
{
  String list = _files;
  int index;

  if (_count == 0)
    return 0;

  size_t n = 0;

  for (index = list.indexOf(','); index != -1; index = list.indexOf(',')) 
  {
    String file = list.substring(1, index - 1);
    files[n++] = file;
    list.remove(0, index + 1);
  }
  
  files[n++] = list.substring(1, list.lastIndexOf("\""));

  return n;
}

uint32_t GSMFileUtils::downloadFile(const String filename, const char buf[], uint32_t size, const bool append)
{
  if (!append)
    deleteFile(filename);

  //Download file +UDWNFILE
  MODEM.downloadFile(filename, size);
  
  MODEM.waitForPrompt(20000);

  //char hexChars[size * 2] = { 0 };
  
  char hexChars[size * 2];
  
  memset(hexChars, 0, sizeof(hexChars));

  for (uint32_t i = 0; i < size; i++) 
  {
    byte b = buf[i];

    byte n1 = (b >> 4) & 0x0f;
    byte n2 = (b & 0x0f);

    hexChars[i * 2]     = (char)(n1 > 9 ? 'A' + n1 - 10 : '0' + n1);
    hexChars[i * 2 + 1] = (char)(n2 > 9 ? 'A' + n2 - 10 : '0' + n2);
  }
  
  for (auto h : hexChars)
    MODEM.write(h);

  int status = MODEM.waitForResponse(1000);
  
  if (status != GSM_RESPONSE_OK)
    return 0;

  auto fileExists = _files.indexOf(filename) > 0;
  
  if (!fileExists) 
  {
    _getFileList();
    _countFiles();
  }

  return size;
}

uint32_t GSMFileUtils::readFile(const String filename, String* content)
{
  String response;

  if (!listFile(filename)) 
  {
    return 0;
  }

  // Read file +URDFILE
  MODEM.readFile(filename);
  
  MODEM.waitForResponse(1000, &response);

  size_t skip = 10;
  String _content = response.substring(skip);

  int commaIndex = _content.indexOf(',');
  skip += commaIndex;

  _content = _content.substring(commaIndex + 1);
  commaIndex = _content.indexOf(',');
  skip += commaIndex;

  String sizePart = _content.substring(0, commaIndex);
  uint32_t size = sizePart.toInt() / 2;
  skip += 3;

  String* _data = content;
  (*_data) = "";
  (*_data).reserve(size);

  for (uint32_t i = 0; i < size; i++) 
  {
    byte n1 = response[skip + i * 2];
    byte n2 = response[skip + i * 2 + 1];
    
    n1 = charToInt(n1);
    n2 = charToInt(n2);

    (*_data) += (char)((n1 << 4) | n2);
  }

  return (*_data).length();
}

uint32_t GSMFileUtils::readFile(const String filename, uint8_t* content)
{
  String response;

  if (listFile(filename) == 0) 
  {
    return 0;
  }

  MODEM.readFile(filename);
  
  MODEM.waitForResponse(1000, &response);

  size_t skip = 10;
  String _content = response.substring(skip);

  int commaIndex = _content.indexOf(',');
  skip += commaIndex;

  _content = _content.substring(commaIndex + 1);
  commaIndex = _content.indexOf(',');
  skip += commaIndex;

  String sizePart = _content.substring(0, commaIndex);
  uint32_t size = sizePart.toInt() / 2;
  skip += 3;

  memset(content, 0, size);

  for (uint32_t i = 0; i < size; i++) 
  {
    byte n1 = response[skip + i * 2];
    byte n2 = response[skip + i * 2 + 1];
    
    n1 = charToInt(n1);
    n2 = charToInt(n2);

    content[i] = (n1 << 4) | n2;
  }

  return size;
}

uint32_t GSMFileUtils::readBlock(const String filename, const uint32_t offset, const uint32_t len, uint8_t* content)
{
  String response;

  if (listFile(filename) == 0) 
  {
    return 0;
  }

  // Partial read file +URDBLOCK
  MODEM.readBlock(filename, offset, len);
  
  MODEM.waitForResponse(1000, &response);

  size_t skip = 10;
  String _content = response.substring(skip);

  int commaIndex = _content.indexOf(',');
  skip += commaIndex;

  _content = _content.substring(commaIndex + 1);
  commaIndex = _content.indexOf(',');
  skip += commaIndex;

  String sizePart = _content.substring(0, commaIndex);
  uint32_t size = sizePart.toInt() / 2;
  skip += 3;

  for (uint32_t i = 0; i < size; i++) 
  {
    byte n1 = response[skip + i * 2];
    byte n2 = response[skip + i * 2 + 1];
    
    n1 = charToInt(n1);
    n2 = charToInt(n2);

    content[i] = (n1 << 4) | n2;
  }

  return size;
}

bool GSMFileUtils::deleteFile(const String filename)
{
  String response;

  if (listFile(filename) == 0)
    return false;

  // Delete file +UDELFILE
  MODEM.deleteFile(filename);
  
  auto status = MODEM.waitForResponse(100, &response);

  if (status == GSM_RESPONSE_IDLE /*0*/)
    return false;

  _getFileList();
  _countFiles();

  return true;
}

int GSMFileUtils::deleteFiles()
{
  int n = 0;
  String files[_count];

  listFiles(files);

  while (_count > 0) 
  {
    n += deleteFile(files[_count - 1]);
  }

  return n;
}

uint32_t GSMFileUtils::listFile(const String filename) const
{
  String response;
  int res;
  uint32_t size = 0;

  // List files information +ULSTFILE. Size of the specified file
  MODEM.fileSize(filename);
  
  res = MODEM.waitForResponse(100, &response);
  
  if (res == GSM_RESPONSE_OK) 
  {
    String content = response.substring(11);
    size = content.toInt();
  }

  return size / 2;
}

uint32_t GSMFileUtils::freeSpace()
{
  String response;
  int res;
  uint32_t size = 0;

  // List files information +ULSTFILE. Remaining free FS space expressed in bytes
  MODEM.freeSpace();
  
  res = MODEM.waitForResponse(100, &response);
  
  if (res == GSM_RESPONSE_OK) 
  {
    String content = response.substring(11);
    size = content.toInt();
  }

  return size;
}

void printFiles(const GSMFileUtils fu)
{
  auto count = fu.fileCount();

  String files[count];

  Serial.print(count);
  Serial.print(count == 1 ? " file" : " files");
  Serial.println(" found.");

  fu.listFiles(files);

  for (auto f : files) 
  {
    Serial.print("File ");
    Serial.print(f);
    Serial.print(" - Size: ");
    Serial.print(fu.listFile(f));
    Serial.println();
  }
}

#endif    // _GSM_FILE_UTILS_HPP
