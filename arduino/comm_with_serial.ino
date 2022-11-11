#include <DS3231.h>

// https://create.arduino.cc/projecthub/electropeak/sd-card-module-with-arduino-how-to-read-write-data-37f390
// https://lastminuteengineers.com/ds3231-rtc-arduino-tutorial/
// https://www.c-sharpcorner.com/article/audio-play-using-sd-card-module-and-arduino/ (doesn't use audio amp)
// https://github.com/arduino-libraries/AudioZero
// https://github.com/NorthernWidget/DS3231/
// https://github.com/DFRobot/DFPlayer-Mini-mp3/

DS3231 Clock;

bool
    h12Flag,
    pmFlag,
    century = false;

void setup() {
  Serial.begin(115200);
  Wire.begin();
}


void loop() {
    // allow epoch time to be set
    if (Serial.available() != 0)
    {
      unsigned long epoch_time = Serial.parseInt();
      if (epoch_time == 0) return;
      
      Clock.setEpoch(epoch_time);
      
      Serial.print("Set new epoch time to ");
      Serial.println(epoch_time);
    }
  
    Serial.print(Clock.getMonth(century));
    Serial.print("-");
    Serial.print(Clock.getDate());
    Serial.print("-");
    Serial.print(Clock.getYear() + 1970);
    
    Serial.print(" ");
    
    Serial.print(Clock.getHour(h12Flag, pmFlag));
    Serial.print(":");
    Serial.print(Clock.getMinute());
    Serial.print(":");
    Serial.println(Clock.getSecond());
 
    delay(100);
}