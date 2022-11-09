#include <SPI.h>
#include <SD.h>
#include <SPI.h>
#include <AudioZero.h>
#include <DS3231.h>

// https://create.arduino.cc/projecthub/electropeak/sd-card-module-with-arduino-how-to-read-write-data-37f390
// https://lastminuteengineers.com/ds3231-rtc-arduino-tutorial/
// https://www.c-sharpcorner.com/article/audio-play-using-sd-card-module-and-arduino/ (doesn't use audio amp)
// https://github.com/arduino-libraries/AudioZero
// https://github.com/NorthernWidget/DS3231/

DS3231 Clock;

bool h12Flag;
bool pmFlag;
bool century = false;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  Serial.print("Initializing SD card... ");
  
  /*if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }*/

  Serial.print("initialization done.");

  Serial.print("Initializing RTC module... ");

  Wire.begin();
    
  Serial.println("initialization done.");

  // playTime(); // for testing
  
  Clock.setEpoch({1668034845UL}, false);    // set epoch time (GMT)
  Clock.setClockMode(false);                // 24-hour mode
}

void playTime() {
  String fileNames[4];
  parseTime(fileNames);

  for (int i = 0; i < 4; i++) {
    if (fileNames[i] != "") {
      File audioFile = SD.open(fileNames[i]);

      if (!audioFile) {
        // if the file didn't open, print an error and stop
        Serial.println("error opening " + fileNames[i]);
        while (true);
      }

      // 44100kHz stereo => 88200 sample rate
      AudioZero.begin(2 * 44100);

      Serial.print("Playing");
  
      // until the file is not finished  
      AudioZero.play(audioFile);
      AudioZero.end();
      
      Serial.println("End of file. Thank you for listening!");

      // not sure if this is needed? was in the example
      while (true);
    }
   
  }
}

// can only open 1 file at a time
void parseTime(String fileNames[4])
{
  // fileNames[0] = "one-twelve"
  // fileNames[1] = "o'" (if needed, otherwise null)
  // fileNames[2] = "clock" OR "one-fifty nine"
  // fileNames[3] = "PM/AM"

  // convert to 12-hour time
  // 0 is actually 12 AM
  if (Clock.getHour(h12Flag, pmFlag) == 0) {
    fileNames[0] = "12.wav";
    fileNames[3] = "AM.wav";
  // 12 is 12 PM
  } else if (Clock.getHour(h12Flag, pmFlag) == 12) {
    fileNames[0] = "12.wav";
    fileNames[3] = "PM.wav";
  // For 13-23, we have to subtract 12
  } else if (Clock.getHour(h12Flag, pmFlag) > 12) {
    fileNames[0] = String(Clock.getHour(h12Flag, pmFlag) - 12) + ".wav";
    fileNames[3] = "PM.wav";
  // For 0-11, we just treat the time as normal
  } else {
    fileNames[0] = String(Clock.getHour(h12Flag, pmFlag)) + ".wav";
    fileNames[3] = "AM.wav";
  }

  if (Clock.getMinute() < 10) {
    fileNames[1] = "o.wav";

    if (Clock.getMinute() == 0)
      fileNames[2] = "clock.wav";
    else
      fileNames[2] = String(Clock.getMinute()) + ".wav";
  } else {
    fileNames[1] = "";
    fileNames[2] = String(Clock.getMinute()) + ".wav";
  }

  Serial.print("\n");

  Serial.print(fileNames[0] + " " + fileNames[1] + " " + fileNames[2] + " " + fileNames[3]);
  Serial.println();
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

    String fileNames[4];

    parseTime(fileNames);
 
    delay(1000);
}
