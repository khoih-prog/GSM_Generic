/*********************************************************************************************************************************
  Helpers.h

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
#pragma once

#ifndef Helpers_h
#define Helpers_h

#include <Arduino.h>

String readLine()
{
  String line;

  while (1)
  {
    if (Serial.available())
    {
      char c = Serial.read();

      if (c == '\r')
      {
        // ignore
      }
      else if (c == '\n')
      {
        break;
      }

      line += c;
    }
  }

  line.trim();

  return line;
}

String promptAndReadLine(const char* prompt, const char* defaultValue) 
{
  Serial.print(prompt);
  Serial.print(" [");
  Serial.print(defaultValue);
  Serial.print("]: ");

  String s = readLine();

  if (s.length() == 0) 
  {
    s = defaultValue;
  }

  Serial.println(s);

  return s;
}

int promptAndReadInt(const char* prompt, const int defaultValue) 
{
  Serial.print(prompt);
  Serial.print(" [");
  Serial.print(defaultValue);
  Serial.print("]: ");

  String s = readLine();
  int r;

  if (s.length() == 0) 
  {
    r = defaultValue;
  } 
  else 
  {
    r = s.toInt();
  }

  Serial.println(r);

  return r;
}

String promptAndReadLine(const char *prompt)
{
  Serial.print(prompt);
  String s = readLine();
  Serial.println(s);

  return s;
}

int promptAndReadInt(const char *prompt)
{
  Serial.print(prompt);
  String s = readLine();
  Serial.println(s);

  return s.toInt();
}


String toHex(char c)
{
  String hex;

  hex = "0x";
  
  if (c < 16)
    hex += "0";
    
  hex += String(c, HEX);

  return hex;
}

void printHex(const String& buf, const unsigned int cols)
{
  for (size_t i = 0; i < buf.length(); i++) 
  {
    String hex = toHex(buf[i]);
    hex += " ";
    Serial.print(hex);
    
    if (i % cols == (cols - 1))
      Serial.println();
  }
  
  Serial.println();
}

void printHex(const uint8_t* buf, const size_t len, const unsigned int cols)
{
  for (size_t i = 0; i < len; i++) 
  {
    //    Serial.print("0x");
    
    if (buf[i] < 16)
      Serial.print(0);
      
    Serial.print(buf[i], HEX);
    
    //    if (i != len - 1) Serial.print(", ");
    if (i % cols == (cols - 1))
      Serial.println();
  }
  
  Serial.println();
}

#endif    // Helpers_h
