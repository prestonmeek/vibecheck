#pragma once
#include <DS3231.h>
#include "DFRobotDFPlayerMini.h"
#include "current_time.h"

// reads the time out loud using DS3231 and DFPlayer Mini
class TimeReader {
public:
    // parses and plays the current date and time out loud
    static void play_time(DS3231& _clock, DFPlayer& _player) {
        auto _time = CurrentTime::get(_clock); // get current time
        FileSequence _file_sequence(_time); // create a file sequence from the time

        _file_sequence.play(_player); // play our file sequence on the DF player
    }
private:
    /*
        SD CARD FORMAT
        Folder 1 has number segments
        Folder 2 has AM/PM (file 1 is AM, file 2 is PM)
        Folder 3 has "o" and "clock" (file 1 is "o", file 2 is "clock")
        Folder 4 has month names
        Folder 5 has weekday names
        Folder 6 has date numbers (i.e. first, second, third, etc.)
    */

    // stores the position of a specific file in a specific folder
    struct File {
        uint8_t
          _folder: 4, // the folder id
          _file_id: 6; // the file id, uses rest of bit but only needs to satisfy 0-59
    };

    // stores information about a sequence of files to play to announce the date and time
    class FileSequence {
    public:
        // parses a file sequence from the current time
        FileSequence(CurrentTime& _time) {
            _sequence[0] = File { 5, _time._weekday + 1 }; // weekday (0=sunday, 1=monday, etc.)
            _sequence[1] = File { 4, _time._month }; // month (1=january, 2=february, etc.)
            _sequence[2] = File { 6, _time._day }; // day of the month

            _sequence[3] = File { 1, _time._hour }; // current hour
            
            _sequence[4] =
              _time._minute < 10 // if current minute is less than 10 say 'o'
                ? File { 3, 1 }
                : File { 0, 0 };
                
            _sequence[5] = File { 1, _time._minute };
            _sequence[6] = File { 2, _time._pm_flag + 1 }; // either AM or PM
        }

        // plays the file sequence out loud
        void play(DFPlayer& _player) {
            _player.playFolder(0, 0); // not sure why we need this but it seems to work
            
            for (uint8_t i = 0; i < 7; i++) { // go through each element of the sequence
                if (_sequence[i]._folder != 0 && _sequence[i]._file_id != 0) { // if this is an actual file
                    _player.playFolder(_sequence[i]._folder, _sequence[i]._file_id); // play the selected file
                    delay(1500); // TODO: connect the BUSY pin of the DFPlayer to a pin of the ATMega and use that method to wait until playback is done
                }
            }
        }
    private:
        // sequence: "[WEEKDAY], [MONTH] [DAY]. [HOUR] [O?] [MINUTE] [AM/PM]."
        File _sequence[7];
    };
};
