
// this was modified from an example in the repo for the DS3231 library

#pragma once
#include <DS3231.h>
#include <Wire.h>

DS3231 rtc;

byte year;
byte month;
byte date;
byte dOW;
byte hour;
byte minute;
byte second;
bool century = false;
bool h12Flag;
bool pmFlag;
byte alarmDay, alarmHour, alarmMinute, alarmSecond, alarmBits;
bool alarmDy, alarmH12Flag, alarmPmFlag;

void getDateStuff(byte& year, byte& month, byte& date, byte& dOW,
                  byte& hour, byte& minute, byte& second) {
    // Call this if you notice something coming in on
    // the serial port. The stuff coming in should be in
    // the order YYMMDDwHHMMSS, with an 'x' at the end.
    boolean gotString = false;
    char inChar;
    byte temp1, temp2;
    char inString[20];
    
    byte j=0;
    while (!gotString) {
        if (Serial.available()) {
            inChar = Serial.read();
            inString[j] = inChar;
            j += 1;
            if (inChar == 'x') {
                gotString = true;
            }
        }
    }
    Serial.println(inString);
    // Read year first
    temp1 = (byte)inString[0] -48;
    temp2 = (byte)inString[1] -48;
    year = temp1*10 + temp2;
    // now month
    temp1 = (byte)inString[2] -48;
    temp2 = (byte)inString[3] -48;
    month = temp1*10 + temp2;
    // now date
    temp1 = (byte)inString[4] -48;
    temp2 = (byte)inString[5] -48;
    date = temp1*10 + temp2;
    // now Day of Week
    dOW = (byte)inString[6] - 48;
    // now hour
    temp1 = (byte)inString[7] -48;
    temp2 = (byte)inString[8] -48;
    hour = temp1*10 + temp2;
    // now minute
    temp1 = (byte)inString[9] -48;
    temp2 = (byte)inString[10] -48;
    minute = temp1*10 + temp2;
    // now second
    temp1 = (byte)inString[11] -48;
    temp2 = (byte)inString[12] -48;
    second = temp1*10 + temp2;
}

const char* days[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
const char* months[12] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };

void printTime()
{
  uint16_t
    month = rtc.getMonth(century),
    day = rtc.getDate(),
    year = 2000 + (century ? 100 : 0) + rtc.getYear(),
    weekday = rtc.getDoW(),
    hour = rtc.getHour(h12Flag, pmFlag),
    minute = rtc.getMinute(),
    second = rtc.getSecond();

  Serial.print("Today is ");
  Serial.print(days[weekday]);
  Serial.print(", ");
  Serial.print(months[month - 1]);
  Serial.print(" ");
  Serial.print(day);
  Serial.print(", ");
  Serial.print(year);
  Serial.print(". The time is ");
  Serial.print(hour);
  Serial.print(":");
  if (minute < 10) Serial.print("0");
  Serial.print(minute);
  Serial.print(":");
  if (second < 10) Serial.print("0");
  Serial.print(second);

  if (h12Flag) {
    if (pmFlag) {
      Serial.print(" PM.");
    } else {
      Serial.print(" AM.");
    }
  } else {
    Serial.print(" 24h.");
  }

  if (!rtc.oscillatorCheck())
    Serial.print(" NOTE: Power may have been disconnected since last check. Time may be inaccurate.");

  Serial.println();
}

void rtcset_loop() {
    // If something is coming in on the serial line, it's
    // a time correction so set the clock accordingly.
    if (Serial.available()) {
        getDateStuff(year, month, date, dOW, hour, minute, second);
        
        rtc.setClockMode(true); // set to 12h
        
        rtc.setYear(year);
        rtc.setMonth(month);
        rtc.setDate(date);
        rtc.setDoW(dOW);
        rtc.setHour(hour);
        rtc.setMinute(minute);
        rtc.setSecond(second);
    }

    printTime();
    delay(1000);
}
