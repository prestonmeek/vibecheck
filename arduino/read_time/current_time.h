#pragma once
#include <DS3231.h>

// holds information about the current time
struct CurrentTime {
    // the below fields are all the information about the time that we need to relay to the user (seconds and years are not required)
    uint8_t
        _month: 4, // the current month (1-12, so only needs 4 bits for a max range of 0-15)
        _hour: 4, // the current hour (1 - 12, so only needs 4 bits for a max range of 0-15)
        _minute: 6, // the current minute (0 - 59, so only needs 6 bits for a max range of 0-63)
        _12_hour_flag: 1, // 12-hour flag, true (1) = uses 12 hour system, false (0) = uses 24 hour system
        _pm_flag: 1, // AM/PM flag, true (1) = PM, false (0) = AM
        _day: 5, // the current day (1-31, so only needs 5 bits to for a max range of 0-31)
        _weekday: 3; // the current day of the week i.e. 1=sunday 2=monday etc (1-7, so only needs 3 bits for a max range of 0-7)

    // communicates with the RTC module to fill out all the required fields
    static CurrentTime get(DS3231& _clock) {
        CurrentTime c_time;

        bool _century = false; // century flag, is ignored
        c_time._month = _clock.getMonth(_century); // month
        c_time._day = _clock.getDate(); // day
        c_time._weekday = _clock.getDoW(); // weekday # (i.e. 1 = sunday, 2 = monday, etc)

        bool _12_hour_flag, _pm_flag; // these are temporary values that will be fed directly back into the struct. This is because you cannot take a reference of a value in a bitfield.
        c_time._hour = _clock.getHour(_12_hour_flag, _pm_flag); // hour, passing the 12hr and pm flags as references to be updated as well
        c_time._minute = _clock.getMinute(); // minute

        // feed flags back into struct
        c_time._12_hour_flag = _12_hour_flag;
        c_time._pm_flag = _pm_flag;

        return c_time;
    }
};
