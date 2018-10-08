// ----------------------------------------------------
//
// APC magazine - Arduino Masterclass
// Project #18 - Digital Audio fileorder v2.0
// Darren Yates - 11 April 2014
//
// Includes the SdFat library - https://code.google.com/p/sdfatlib/downloads/list
//
// ----------------------------------------------------

#include "SdFat.h"
#include <SPI.h>
#include "FreeStack.h"

SdFat sd;
SdFile file;

// Serial output stream
ArduinoOutStream cout(Serial);

// SD chip select pin
const uint8_t chipSelect = 10;

const int SDchipSelect = 10;
unsigned long fileSize = 0L;
unsigned long waveChunk = 16;
unsigned int waveType = 1;
unsigned int numChannels = 1;
unsigned long sampleRate = 22050;
unsigned long bytesPerSec = 22050;
unsigned int blockAlign = 1;
unsigned int bitsPerSample = 8;
unsigned long dataSize = 0L;
unsigned long fileByteCount = 0L;
unsigned long fileByteSaved = 0L;
const int btn = 2;
const int ledStart = 13;
unsigned long oldTime = 0L;
unsigned long newTime = 0L;
byte buf00[512]; // buffer array 1
byte buf01[512]; // buffer array 2
byte byte1, byte2, byte3, byte4;
unsigned int bufByteCount;
byte bufWrite;

int fileORDING  = 0;

//declare const for window mode settings
const byte DISABLE = 0;
const byte MODE1 = 1; //RESULT > Lower Tolerance
const byte MODE2 = 2; //RESULT < Upper Tolerance
const byte MODE3 = 3;
const byte MODE4 = 4;
int adcvalue = 0;

//------------------------------------------------------------------------------
// Store error strings in flash to save RAM.
#define error(s) sd.errorHalt(F(s))
//------------------------------------------------------------------------------
void cidDmp() {
  cid_t cid;
  if (!sd.card()->readCID(&cid)) {
    error("readCID failed");
  }
  cout << F("\nManufacturer ID: ");
  cout << hex << int(cid.mid) << dec << endl;
  cout << F("OEM ID: ") << cid.oid[0] << cid.oid[1] << endl;
  cout << F("Product: ");
  for (uint8_t i = 0; i < 5; i++) {
    cout << cid.pnm[i];
  }
  cout << F("\nVersion: ");
  cout << int(cid.prv_n) << '.' << int(cid.prv_m) << endl;
  cout << F("Serial number: ") << hex << cid.psn << dec << endl;
  cout << F("Manufacturing date: ");
  cout << int(cid.mdt_month) << '/';
  cout << (2000 + cid.mdt_year_low + 10 * cid.mdt_year_high) << endl;
  cout << endl;
}

void setup() { // THIS RUNS ONCE
   Serial.begin(9600);

   
//  pinMode(10, OUTPUT);
    pinMode(ledStart, OUTPUT);
    pinMode(btn,INPUT_PULLUP);

  Serial.println("here  :" + String(SS));

  if (!sd.begin(chipSelect, SD_SCK_MHZ(50))) {
    sd.initErrorHalt();
  }
  
  cidDmp();
  writeWavHeader();
}

void loop() { // THIS RUNS LOTS!
    delay(250);
}


void writeOutHeader() { // update WAV header with final filesize/datasize

  file.seekSet(4);
  byte1 = fileByteSaved & 0xff;
  byte2 = (fileByteSaved >> 8) & 0xff;
  byte3 = (fileByteSaved >> 16) & 0xff;
  byte4 = (fileByteSaved >> 24) & 0xff;  
  file.write(byte1);  file.write(byte2);  file.write(byte3);  file.write(byte4);
  file.seekSet(40);
  file.write(byte1);  file.write(byte2);  file.write(byte3);  file.write(byte4);
  file.close();
}

void writeWavHeader() { // write out original WAV header to file

  Serial.println("here 1 ");
  fileByteSaved = 0;
  if ( !file.open("file00000.wav", O_CREAT | O_TRUNC | O_RDWR) ) {
    Serial.println("Error : File open failed");
  }
  Serial.println("here 2");
  file.write("RIFF");
  byte1 = fileSize & 0xff;
  byte2 = (fileSize >> 8) & 0xff;
  byte3 = (fileSize >> 16) & 0xff;
  byte4 = (fileSize >> 24) & 0xff;  
  file.write(byte1);  file.write(byte2);  file.write(byte3);  file.write(byte4);
  file.write("WAVE");
  file.write("fmt ");
  byte1 = waveChunk & 0xff;
  byte2 = (waveChunk >> 8) & 0xff;
  byte3 = (waveChunk >> 16) & 0xff;
  byte4 = (waveChunk >> 24) & 0xff;  
  file.write(byte1);  file.write(byte2);  file.write(byte3);  file.write(byte4);
  byte1 = waveType & 0xff;
  byte2 = (waveType >> 8) & 0xff;
  file.write(byte1);  file.write(byte2); 
  byte1 = numChannels & 0xff;
  byte2 = (numChannels >> 8) & 0xff;
  file.write(byte1);  file.write(byte2); 
  byte1 = sampleRate & 0xff;
  byte2 = (sampleRate >> 8) & 0xff;
  byte3 = (sampleRate >> 16) & 0xff;
  byte4 = (sampleRate >> 24) & 0xff;  
  file.write(byte1);  file.write(byte2);  file.write(byte3);  file.write(byte4);
  byte1 = bytesPerSec & 0xff;
  byte2 = (bytesPerSec >> 8) & 0xff;
  byte3 = (bytesPerSec >> 16) & 0xff;
  byte4 = (bytesPerSec >> 24) & 0xff;  
  file.write(byte1);  file.write(byte2);  file.write(byte3);  file.write(byte4);
  byte1 = blockAlign & 0xff;
  byte2 = (blockAlign >> 8) & 0xff;
  file.write(byte1);  file.write(byte2); 
  byte1 = bitsPerSample & 0xff;
  byte2 = (bitsPerSample >> 8) & 0xff;
  file.write(byte1);  file.write(byte2); 
  file.write("data");
  byte1 = dataSize & 0xff;
  byte2 = (dataSize >> 8) & 0xff;
  byte3 = (dataSize >> 16) & 0xff;
  byte4 = (dataSize >> 24) & 0xff;  
  file.write(byte1);  file.write(byte2);  file.write(byte3);  file.write(byte4);

}

