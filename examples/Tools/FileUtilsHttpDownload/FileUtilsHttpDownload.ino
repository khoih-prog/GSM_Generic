/*********************************************************************************************************************************
  FileUtilsHttpDownload.ino

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
  Download large file and store it into the GSM module filesystem.

  This sketch connects to a website through a GSM module and downloads a large file and stores it into the filesystem of the GSM
  module.

  The file is processed in blocks of 512 bytes in order to save RAM. A block of data is read from the GSM module and
  then appended to a file created by the sketch.

  Circuit:
    GSM Module
   Antenna
   SIM card with a data plan

  created 19 June 2020
  by Giampaolo Mancini
*/

#include "defines.h"

GSMFileUtils fileUtils(false);

#include "Helpers.h"

// Please enter your sensitive data in the Secret tab or arduino_secrets.h
// PIN Number
const char PINNUMBER[] = SECRET_PINNUMBER;
// APN data
const char GPRS_APN[] = SECRET_GPRS_APN;
const char GPRS_LOGIN[] = SECRET_GPRS_LOGIN;
const char GPRS_PASSWORD[] = SECRET_GPRS_PASSWORD;

// BaudRate to communicate to GSM/GPRS modem. If be limit to max 115200 inside modem
unsigned long baudRateSerialGSM  = 115200;

// initialize the library instance
GSMClient client;
GPRS gprs;
GSM gsmAccess;

// URL, path and port (for example: example.org)

void storeFileBuffered(String filename, uint32_t totalLen)
{
  Serial.print("Ready to download \"");
  Serial.print(filename);
  Serial.print("\" - len: ");
  Serial.print(totalLen);
  Serial.println(" bytes.");

  constexpr uint32_t len { 512 };

  uint32_t cycles = totalLen / len;
  uint32_t spares = totalLen % len;

  int totalRead { 0 };

  fileUtils.deleteFile(filename);

  Serial.print("Saving file in ");
  Serial.print(cycles + 1);
  Serial.print(" blocks. [");
  Serial.print(cycles);
  Serial.print(' ');
  Serial.print(len);
  Serial.print(" -bytes blocks and ");
  Serial.print(spares);
  Serial.println(" bytes].");

  bool is_header_complete = false;
  String http_header;

  // Skip the HTTP header
  while (!is_header_complete)
  {
    while (client.available())
    {
      const char c = client.read();
      http_header += c;

      if (http_header.endsWith("\r\n\r\n"))
      {
        Serial.println("Header Complete");
        is_header_complete = true;
        break;
      }
    }
  }

  // Define download and save lambda
  auto downloadAndSaveTrunk = [filename](uint32_t len)
  {
    char buf[len];
    uint32_t written = { 0 };

    memset(buf, 0, sizeof(buf));

    if (client.available())
      written = client.readBytes(buf, len);

    fileUtils.appendFile(filename, buf, written);
    return written;
  };

  // Define wrapper function
  auto saveTrunk = [&totalRead, downloadAndSaveTrunk](size_t iter, uint32_t len)
  {
    Serial.print("Block ");

    if (iter < 10)
      Serial.print(' ');

    if (iter < 100)
      Serial.print(' ');

    Serial.print(iter);

    totalRead += downloadAndSaveTrunk(len);

    Serial.print(": ");
    Serial.print(len);
    Serial.print(" - ");
    Serial.print(totalRead);
    Serial.println();
  };

  // Download and save complete trunks + spares
  for (uint32_t c = 0; c <= cycles; c++)
    saveTrunk(c, len);

  Serial.println();
}

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial);

  delay(200);

  Serial.print(F("\nStarting FileUtilsHttpDownload on ")); Serial.println(BOARD_NAME);
  Serial.println(GSM_GENERIC_VERSION);

#if ( defined(DEBUG_GSM_GENERIC_PORT) && (_GSM_GENERIC_LOGLEVEL_ > 4) )
  MODEM.debug(DEBUG_GSM_GENERIC_PORT);
#endif  

  Serial.println("Starting Arduino web client.");

  fileUtils.begin(baudRateSerialGSM);

  // List files on the GSM module's filesystem
  auto numberOfFiles = fileUtils.fileCount();
  Serial.print("Number of Files: ");
  Serial.println(numberOfFiles);
  Serial.println();

  printFiles(fileUtils);

  auto server   = promptAndReadLine("Please, enter server name:", "arduino.cc");
  auto port     = promptAndReadInt("Please, enter server port:", 80);
  auto filename = promptAndReadLine("Please, enter file name:", "asciilogo.txt");
  auto filesize = promptAndReadInt("Please, enter file size:", 2263);

  Serial.println("Connecting...");

  // connection state
  bool connected = false;

  // After starting the modem with GSM.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while (!connected)
  {
    if ((gsmAccess.begin(baudRateSerialGSM, PINNUMBER) == GSM_READY) && 
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY))
    {
      connected = true;
    }
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  // if you get a connection, report back via serial:
  if (client.connect(server.c_str(), port))
  {
    Serial.println("connected");
    // Make a HTTP request:
    client.print("GET /");
    client.print(filename);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();
  }
  else
  {
    // if you didn't get a connection to the server:
    Serial.println("Connection failed");
  }

  // Download and store block-by-block
  storeFileBuffered(filename, filesize);

  auto updateBinSize = fileUtils.listFile(filename);
  Serial.print(filename);
  Serial.print(" downloaded size: ");
  Serial.println(updateBinSize);

  numberOfFiles = fileUtils.fileCount();
  Serial.print("Number of Files: ");
  Serial.println(numberOfFiles);
  Serial.println();

  printFiles(fileUtils);
}

void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them:
  if (client.available())
  {
    char r = client.read();

    if (r < 16)
      Serial.print(0);

    Serial.print(r, HEX);
  }

  // if the server's disconnected, stop the client:
  if (!client.available() && !client.connected())
  {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    for (;;);
  }
}
