#include <SPI.h>
#include <SD.h>
#include <uRTCLib.h>
#include <SPI.h>
#include <AudioZero.h>

// https://create.arduino.cc/projecthub/electropeak/sd-card-module-with-arduino-how-to-read-write-data-37f390
// https://lastminuteengineers.com/ds3231-rtc-arduino-tutorial/
// https://www.c-sharpcorner.com/article/audio-play-using-sd-card-module-and-arduino/ (doesn't use audio amp)
// https://github.com/arduino-libraries/AudioZero

uRTCLib rtc(0x68);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  
  Serial.print("Initializing SD card... ");
  
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }

  Serial.print("Initializing RTC module... ");

  URTCLIB_WIRE.begin();
  rtc.set(0, 56, 12, 5, 13, 1, 22);
    
  Serial.println("initialization done.");

  playTime(); // for testing
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
  if (rtc.hour() > 12) {
    fileNames[0] = String(rtc.hour() - 12) + ".wav";
    fileNames[3] = "PM.wav";
  } else {
    fileNames[0] = String(rtc.hour()) + ".wav";
    fileNames[3] = "AM.wav";
  }

  if (rtc.minute() < 10) {
    fileNames[1] = "o.wav";

    if (rtc.minute() == 0)
      fileNames[2] = "clock.wav";
    else
      fileNames[2] = String(rtc.minute()) + ".wav";
  } else {
    fileNames[1] = "";
    fileNames[2] = String(rtc.minute()) + ".wav";
  }
}

void loop() {
  rtc.refresh();
  delay(1000);
}
