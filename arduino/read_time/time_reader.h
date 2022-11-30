#pragma once
#include <DS3231.h>
#include "DFRobotDFPlayerMini.h"
#include "current_time.h"

// reads the time out loud using DS3231 and DFPlayer Mini
class TimeReader {
public:
    // parses and plays the current date and time out loud
    static void play_time(DS3231& _clock, DFPlayer& _player) {
        auto _time = CurrentTime::get(_clock);
        FileSequence _file_sequence(_time);

        _file_sequence.play(_player);
    }
private:
    /*
        SD CARD FORMAT:
        fileNames[0] = "one-twelve"
        fileNames[1] = "o'" (if needed, otherwise null)
        fileNames[2] = "clock" OR "one-fifty nine"
        fileNames[3] = "PM/AM"

        Structure of each element: [folder number, file number]
        Folder 1 has number segments
        Folder 2 has AM/PM (file 1 is AM, file 2 is PM)
        Folder 3 has "o" and "clock" (file 1 is "o", file 2 is "clock")
    */

    // stores the position of a specific file in a specific folder
    struct File {
        uint8_t
                _folder: 2, // the folder id, (0-2, so only needs 2 bits for a max range of 0-3)
        _file_id: 6; // the file id, uses rest of bit but only needs to satisfy 0-59
    };

    // stores information about a sequence of files to play to announce the date and time
    class FileSequence {
    public:
        // parses a file sequence from the current time
        FileSequence(CurrentTime& _time) {
            _sequence[0] = File { 0, 0 }; // weekday, temporary
            _sequence[1] = File { 0, _time._month }; // month, temporary
            _sequence[2] = File { 0, _time._day }; // day, temporary

            _sequence[3] = File { 0, _time._hour }; // current hour
            _sequence[4] = _time._minute < 10 ? File { 3, 1 } : File { 0, 0 }; // if current minute is less than 10 say 'o'
            _sequence[5] = File { 0, _time._minute };

            _sequence[6] = File { 1, _time._pm_flag + 1 }; // either AM or PM
        }

        // plays the file sequence out loud
        void play(DFPlayer& _player) {
            for (uint8_t i = 0; i < 7; i++) { // go through each element of the sequence
                if (_sequence[i]._folder != 0 || _sequence[i]._file_id != 0) { // if this is an actual file
                    _player.playFolder(_sequence[i]._folder, _sequence[i]._file_id); // play the selected file
                    delay(1300); // TODO: connect the BUSY pin of the DFPlayer to a pin of the ATMega and use that method to wait until playback is done
                }
            }
        }
    private:
        // sequence: "[WEEKDAY], [MONTH] [DAY]. [HOUR] [O?] [MINUTE] [AM/PM]."
        File _sequence[7];
    };
};