microbian port to samd21 specifically trinketm0 at this point.
Relies on code culled from Adafruit arduino impementation.
So right now is heavily polluted with CMSIS code unlike the original microbit-v1 microbian.


Things to note
==============
polling uart TX pieced together from SERCOM.cpp class bodged into c functions.
timer uses systick, 1ms tick, not investigated slower, using 2ms TICK in timer.

TC5 originally used for Tone.cpp could be adapted most easily for Timer if wanted. 

currently programming app using adafruit bootloader v2.0 as supplied preprogrammed.

Not yet ensured that touched files get recompiled before linking app.

Therefore safest useage:
cd microbian
make clean
cd ..
cd blink
make clean
make

#run bossac with make flash assumes bootloader provides /dev/ttyACM0
#I'm using arduino environment for bossac
make flash

Plan to add "C" tinyusb stack as some point for CDC. There was provision in the Adafruit "CPP" port for CDC forced reboot
by connect and disconnect at 1200 baud. See arduino systick for hooks to Reset.cpp.

