#include <SPI.h>
#include <SD.h>
#include <SPI.h>
#include <DS3231.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#define DFPlayer DFRobotDFPlayerMini
#include "print_time.h"
#include "time_reader.h"

SoftwareSerial* softwareSerial; // This software serial instance is for communication with DFPlayer Mini
DFPlayer* Player; // For communicating with DFPlayer Mini, uses above software serial
DS3231* Clock; // For communicating with the DS3231 RTC Module

void setup() {
  // auto-off is tied to pin 7. digitalWrite is called first for speed
  digitalWrite(7, HIGH);
  pinMode(7, OUTPUT);

  // switched to using pointers to save as much time as possible to get pin 7 high
  softwareSerial = new SoftwareSerial(2, 3);
  Player = new DFPlayer;
  Clock = new DS3231;

  // start debug serial
  Serial.begin(9600);
  Serial.print("Initializing SD card... ");
  
  // DFPlayer Mini uses a 9600 baud rate.
  softwareSerial->begin(9600);
  if (!Player->begin(*softwareSerial)) {
    Serial.println("Initialization failed!");
  }

  Player->volume(30);  // 0-30

  Serial.print("Initializing RTC module... ");
  Wire.begin(); // begins I2C comm
  Serial.println("initialization done.");

  Serial.println("Playing audio files...");
  TimeReader::play_time(*Clock, *Player);

  Serial.println("Audio done. Powering off");

  delay(10);
  digitalWrite(7, LOW); // turns circuit off
  delay(10);

  // if the circuit is still on, then the button is being held, indicating the user wants to enter programming mode.
  Serial.println("Entering programming mode...");

  // continue in the loop() function
}

void loop() {
    // listens for time set input (format: YYMMDDWHHMMSS then an 'x', where YY is year, MM is month, DD is day, W is the day of week (0=sun, 1=mon, ...), HH is hours (24hr format), MM is minutes, and SS is seconds)
    // also prints out the time every second for debug purposes
    rtcset_loop();
}
