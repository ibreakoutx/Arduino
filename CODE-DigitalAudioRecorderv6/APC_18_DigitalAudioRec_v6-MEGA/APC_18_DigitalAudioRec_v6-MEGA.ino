// ----------------------------------------------------
//
// APC magazine - Arduino Masterclass
// Project #18 - Digital Audio Recorder v6.0
// Darren Yates - 13 May 2014
//
// ----------------------------------------------------

#include <SdFat.h>
#include "SPI.h"

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

SdFat sd;
SdFile rec;

const unsigned int darBufSize = 128;
const unsigned int totalBuf = darBufSize * 8;
const unsigned int bufSplit_1 = darBufSize;
const unsigned int bufSplit_2 = darBufSize * 2;
const unsigned int bufSplit_3 = darBufSize * 3;
const unsigned int bufSplit_4 = darBufSize * 4;
const unsigned int bufSplit_5 = darBufSize * 5;
const unsigned int bufSplit_6 = darBufSize * 6;
const unsigned int bufSplit_7 = darBufSize * 7;

const int chipSelect = 4;
unsigned long fileSize = 0L;
unsigned long waveChunk = 16;
unsigned int waveType = 1;
unsigned int numChannels = 1;

//------------ SET SAMPLE RATE HERE --------------------
unsigned long sampleRate = 44100;
//unsigned long sampleRate = 22050;
//------------ SET SAMPLE RATE HERE --------------------

unsigned long bytesPerSec = sampleRate;
unsigned int blockAlign = 1;
unsigned int bitsPerSample = 8;
unsigned long dataSize = 0L;
unsigned long recByteCount = 0L;
unsigned long recByteSaved = 0L;

const int btnStart = 6;
const int btnStop = 5;
const int ledStart = 13;
const int ledStop = 3;

//Increment filenum each time record key is pressed
int filenum=0;

int recPressed = 0;
int stopPressed = 0;
unsigned long oldTime = 0L;
unsigned long newTime = 0L;
byte buf00[darBufSize]; // buffer array 1
byte buf01[darBufSize]; // buffer array 2
byte buf02[darBufSize]; // buffer array 3
byte buf03[darBufSize]; // buffer array 4
byte buf04[darBufSize]; // buffer array 4
byte buf05[darBufSize]; // buffer array 4
byte buf06[darBufSize]; // buffer array 4
byte buf07[darBufSize]; // buffer array 4
byte byte1, byte2, byte3, byte4;
unsigned int bufByteCount;
byte bufWrite;
byte lastBuf = 7;
byte valOCR2A = 16000000/(8*sampleRate);

void setup() { // THIS RUNS ONCE

  Serial.begin(9600);
  Setup_timer2();
  Setup_ADC();
  pinMode(10, OUTPUT);
  
  pinMode(ledStart, OUTPUT);
  pinMode(ledStop, OUTPUT);
  
  pinMode(btnStop, INPUT_PULLUP);
  pinMode(btnStart, INPUT_PULLUP);
  
  if (sd.begin(chipSelect, SPI_FULL_SPEED)) { // initialise card on SPI to 8MHz SPI bus speed
    Serial.println("SD Card initialized successfully");
    for (int dloop = 0; dloop < 4; dloop++) {
      digitalWrite(ledStart,!digitalRead(ledStart));
      delay(100);
    }
  } else { // if error, flash LED twice per second, until reset
    Serial.println("SD Card initialization ERROR");
    while(1) {
      digitalWrite(ledStart,!digitalRead(ledStart));
      digitalWrite(ledStop,!digitalRead(ledStop));
      delay(500);
    }
  }

}

void loop() { // THIS RUNS LOTS!

  if (digitalRead(btnStart) == LOW && recPressed == 0) {
    Serial.println("++++++++ START RECORDING +++++++");
    StartRec(); // launch StartRec method
  }
  if (digitalRead(btnStop) == LOW && recPressed == 1) {
    Serial.println("********* STOP RECORDING*****");
    StopRec(); // launch StopRec method
  }
  
  if (recPressed == 1) {
    if (recByteCount % totalBuf > bufSplit_1 && lastBuf == 7) { 
      rec.write(buf00,darBufSize); lastBuf = 0; recByteSaved += darBufSize; 
    } else if (recByteCount % totalBuf > bufSplit_2 && lastBuf == 0) {
      rec.write(buf01,darBufSize); lastBuf = 1; recByteSaved += darBufSize; 
    } else if (recByteCount % totalBuf > bufSplit_3 && lastBuf == 1) { 
      rec.write(buf02,darBufSize); lastBuf = 2; recByteSaved += darBufSize; 
    } else if (recByteCount % totalBuf > bufSplit_4 && lastBuf == 2) { 
      rec.write(buf03,darBufSize); lastBuf = 3; recByteSaved += darBufSize; 
    } else if (recByteCount % totalBuf > bufSplit_5 && lastBuf == 3) { 
      rec.write(buf04,darBufSize); lastBuf = 4; recByteSaved += darBufSize; 
    } else if (recByteCount % totalBuf > bufSplit_6 && lastBuf == 4) { 
      rec.write(buf05,darBufSize); lastBuf = 5; recByteSaved += darBufSize; 
    } else if (recByteCount % totalBuf > bufSplit_7 && lastBuf == 5) { 
      rec.write(buf06,darBufSize); lastBuf = 6; recByteSaved += darBufSize; 
    } else if (recByteCount % totalBuf < bufSplit_1 && lastBuf == 6) { 
      rec.write(buf07,darBufSize); lastBuf = 7; recByteSaved += darBufSize; 
    }
  }  
}

void StartRec() { // begin recording process

  digitalWrite(ledStart,HIGH);
  digitalWrite(ledStop,LOW);
  recByteCount = 0;
  bufByteCount = 0;
  recByteSaved = 0;
  bufWrite = 0;
  recPressed = 1; // recording button has been pressed
  stopPressed = 0;
  writeWavHeader();
  sbi (TIMSK2, OCIE2A); // enable timer interrupt, start grabbing audio

}

void StopRec() { // stop recording process, update WAV header, close file
    
  cbi (TIMSK2, OCIE2A); // disable timer interrupt
  writeOutHeader();
  digitalWrite(ledStart,LOW); // turn off recording LED
  digitalWrite(ledStop,HIGH); // light stop LED
  recPressed = 0;
  Serial.println("List of files:");
  sd.ls(LS_R);
  
}
  
void writeOutHeader() { // update WAV header with final filesize/datasize

  rec.seekSet(4);
  byte1 = recByteSaved & 0xff;
  byte2 = (recByteSaved >> 8) & 0xff;
  byte3 = (recByteSaved >> 16) & 0xff;
  byte4 = (recByteSaved >> 24) & 0xff;  
  rec.write(byte1);  rec.write(byte2);  rec.write(byte3);  rec.write(byte4);
  rec.seekSet(40);
  rec.write(byte1);  rec.write(byte2);  rec.write(byte3);  rec.write(byte4);
  rec.close();
  
}

void writeWavHeader() { // write out original WAV header to file

  String filename ;
  recByteSaved = 0;
  
  filenum++;

  filename = String("rec") + String(filenum) + String(".wav");
  
  rec.open(filename.c_str(), O_CREAT | O_TRUNC | O_RDWR);

  Serial.println("Opened file:" + filename + " for recording");
  
  rec.write("RIFF");
  byte1 = fileSize & 0xff;
  byte2 = (fileSize >> 8) & 0xff;
  byte3 = (fileSize >> 16) & 0xff;
  byte4 = (fileSize >> 24) & 0xff;  
  rec.write(byte1);  rec.write(byte2);  rec.write(byte3);  rec.write(byte4);
  rec.write("WAVE");
  rec.write("fmt ");
  byte1 = waveChunk & 0xff;
  byte2 = (waveChunk >> 8) & 0xff;
  byte3 = (waveChunk >> 16) & 0xff;
  byte4 = (waveChunk >> 24) & 0xff;  
  rec.write(byte1);  rec.write(byte2);  rec.write(byte3);  rec.write(byte4);
  byte1 = waveType & 0xff;
  byte2 = (waveType >> 8) & 0xff;
  rec.write(byte1);  rec.write(byte2); 
  byte1 = numChannels & 0xff;
  byte2 = (numChannels >> 8) & 0xff;
  rec.write(byte1);  rec.write(byte2); 
  byte1 = sampleRate & 0xff;
  byte2 = (sampleRate >> 8) & 0xff;
  byte3 = (sampleRate >> 16) & 0xff;
  byte4 = (sampleRate >> 24) & 0xff;  
  rec.write(byte1);  rec.write(byte2);  rec.write(byte3);  rec.write(byte4);
  byte1 = bytesPerSec & 0xff;
  byte2 = (bytesPerSec >> 8) & 0xff;
  byte3 = (bytesPerSec >> 16) & 0xff;
  byte4 = (bytesPerSec >> 24) & 0xff;  
  rec.write(byte1);  rec.write(byte2);  rec.write(byte3);  rec.write(byte4);
  byte1 = blockAlign & 0xff;
  byte2 = (blockAlign >> 8) & 0xff;
  rec.write(byte1);  rec.write(byte2); 
  byte1 = bitsPerSample & 0xff;
  byte2 = (bitsPerSample >> 8) & 0xff;
  rec.write(byte1);  rec.write(byte2); 
  rec.write("data");
  byte1 = dataSize & 0xff;
  byte2 = (dataSize >> 8) & 0xff;
  byte3 = (dataSize >> 16) & 0xff;
  byte4 = (dataSize >> 24) & 0xff;  
  rec.write(byte1);  rec.write(byte2);  rec.write(byte3);  rec.write(byte4);

}

void Setup_timer2() {

  TCCR2B = _BV(CS21);  // Timer2 Clock Prescaler to : 8
  TCCR2A = _BV(WGM21); // Interupt frequency  = 16MHz / (8 x 62 + 1) ~= 32000Hz
  OCR2A = valOCR2A; // Compare Match register set to 62

}

void Setup_ADC() {

  ADMUX = 0x65; // set ADC to read pin A5, ADLAR to 1 (left adjust)
  cbi(ADCSRA,ADPS2); // set prescaler to 3 / ADC clock = 4MHz
  sbi(ADCSRA,ADPS1);
  sbi(ADCSRA,ADPS0);

}

ISR(TIMER2_COMPA_vect) {

  sbi(ADCSRA, ADSC); // start ADC sample
  while(bit_is_set(ADCSRA, ADSC));  // wait until ADSC bit goes low = new sample ready
  recByteCount++; // increment sample counter

  if (recByteCount % totalBuf < bufSplit_1) {
    buf00[recByteCount % darBufSize] = ADCH;
  } else if (recByteCount % totalBuf < bufSplit_2) { 
    buf01[recByteCount % darBufSize] = ADCH;
  } else if (recByteCount % totalBuf < bufSplit_3) {
    buf02[recByteCount % darBufSize] = ADCH;
  } else if (recByteCount % totalBuf < bufSplit_4) {
    buf03[recByteCount % darBufSize] = ADCH;
  } else if (recByteCount % totalBuf < bufSplit_5) {
    buf04[recByteCount % darBufSize] = ADCH;
  } else if (recByteCount % totalBuf < bufSplit_6) {
    buf05[recByteCount % darBufSize] = ADCH;
  } else if (recByteCount % totalBuf < bufSplit_7) {
    buf06[recByteCount % darBufSize] = ADCH;
  } else {
    buf07[recByteCount % darBufSize] = ADCH;
  }

}
