#include <SPI.h>
#include <SD.h>
#include <SPI.h>
#include <DS3231.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// https://create.arduino.cc/projecthub/electropeak/sd-card-module-with-arduino-how-to-read-write-data-37f390
// https://lastminuteengineers.com/ds3231-rtc-arduino-tutorial/
// https://www.c-sharpcorner.com/article/audio-play-using-sd-card-module-and-arduino/ (doesn't use audio amp)
// https://github.com/arduino-libraries/AudioZero
// https://github.com/NorthernWidget/DS3231/
// https://github.com/DFRobot/DFPlayer-Mini-mp3/

SoftwareSerial softwareSerial(11, 10);

// Create the Player object
DFRobotDFPlayerMini Player;

// Create the RTC object
DS3231 Clock;

bool h12Flag;
bool pmFlag;
bool century = false;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  Serial.print("Initializing SD card... ");
  
  // Init serial port for DFPlayer Mini
  softwareSerial.begin(9600);
  if (!Player.begin(softwareSerial)) {
    Serial.println("Initialization failed!");
  }

  Player.volume(25);  // 0-30

  Serial.println("initialization done.");

  Serial.print("Initializing RTC module... ");

  Wire.begin();
    
  Serial.println("initialization done.");
  
  Clock.setEpoch({1668600035UL}, false);    // set epoch time (GMT)
  Clock.setClockMode(false);                // 24-hour mode
}

void playTime() {
  int fileNames[4][2];
  parseTime(fileNames);

  for (int i = 0; i < 4; i++) {
    if (fileNames[i][0] != 0 && fileNames[i][1] != 0) {
      Player.playFolder(fileNames[i][0], fileNames[i][1]);
      delay(1300);
    }
   
  }
}

// can only open 1 file at a time
void parseTime(int fileNames[4][2])
{
  /*
    FORMAT:
    fileNames[0] = "one-twelve"
    fileNames[1] = "o'" (if needed, otherwise null)
    fileNames[2] = "clock" OR "one-fifty nine"
    fileNames[3] = "PM/AM"
  
    Structure of each element: [folder number, file number]
    Folder 1 has number segments
    Folder 2 has AM/PM (file 1 is AM, file 2 is PM)
    Folder 3 has "o" and "clock" (file 1 is "o", file 2 is "clock")
  */

  // convert to 12-hour time
  // 0 is actually 12 AM
  if (Clock.getHour(h12Flag, pmFlag) == 0) {
    storeFileSequence(fileNames[0], 1, 12); // 12
    storeFileSequence(fileNames[3], 2, 1);  // AM
  // 12 is 12 PM
  } else if (Clock.getHour(h12Flag, pmFlag) == 12) {
    storeFileSequence(fileNames[0], 1, 12); // 12
    storeFileSequence(fileNames[3], 2, 2);  // PM
  // For 13-23, we have to subtract 12
  } else if (Clock.getHour(h12Flag, pmFlag) > 12) {
    storeFileSequence(fileNames[0], 1, Clock.getHour(h12Flag, pmFlag) - 12); // hour
    storeFileSequence(fileNames[3], 2, 2);  // PM
  // For 0-11, we just treat the time as normal
  } else {
    storeFileSequence(fileNames[0], 1, Clock.getHour(h12Flag, pmFlag) - 12); // hour
    storeFileSequence(fileNames[3], 2, 1);  // AM
  }

  if (Clock.getMinute() < 10)
    storeFileSequence(fileNames[1], 3, 1);  // "o"
  else
    storeFileSequence(fileNames[1], 0, 0);  // null

  if (Clock.getMinute() == 0)
    storeFileSequence(fileNames[2], 3, 2);  // "clock"
  else
    storeFileSequence(fileNames[2], 1, Clock.getMinute());  // minutes

  Serial.println();
}

void storeFileSequence(int oldSequence[2], int val1, int val2)
{
  oldSequence[0] = val1;
  oldSequence[1] = val2;
}

void loop() {
    // Just for verification of DS3231 Data
    // check now the data from ESP8266 and DS3231
    // get year
    
    Serial.print("\n\n");
    Serial.print(" DateTime of DS3231:     ");
    Serial.print(Clock.getYear(), DEC);
    Serial.print("-");
    Serial.print(Clock.getMonth(century), DEC);
    Serial.print("-");
    Serial.print(Clock.getDate(), DEC);
    Serial.print(" ");
    Serial.print(Clock.getHour(h12Flag, pmFlag), DEC);
    Serial.print(":");
    Serial.print(Clock.getMinute(), DEC);
    Serial.print(":");
    Serial.print(Clock.getSecond(), DEC);
    Serial.print("  -  weekday ");
    Serial.print(Clock.getDoW(), DEC);
    Serial.println();

    int fileNames[4][2];

    /*parseTime(fileNames);

    Serial.print(fileNames[0][1], DEC);
    Serial.print("-");
    Serial.print(fileNames[1][1], DEC);
    Serial.print("-");
    Serial.print(fileNames[2][1], DEC);
    Serial.print("-");
    Serial.print(fileNames[3][1], DEC);*/

    playTime();
 
    delay(3000);
}
