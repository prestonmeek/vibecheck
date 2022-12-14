minipro -p "ATMEGA328P@DIP28" -f ihex -i -w "read_time/read_time.ino.arduino_standard.hex" &&
minipro -p "ATMEGA328P@DIP28" -f ihex -i -m "read_time/read_time.ino.arduino_standard.hex" &&
echo Done
