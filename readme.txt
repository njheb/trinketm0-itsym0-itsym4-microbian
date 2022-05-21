Microbian nrf51/52 ported to Adafruit SAMD21/51 (early work)
============================================================

Supported Boards so far: Trinket_m0, ItsyBitsy_m0, ItsyBitsy_m4

Up until this point there were 3 parallel trees. Most development was for
trinket_m0 next came itsy_m0 and finally itsy_m4.

They have now been combined into a single tree. The code is derived from Adafruit SAMD arduino. 
I don't intend to replace the CMSIS code to make it much more like the nrf51/52 trees.

You will need to install an arduino IDE to have access to CMSIS include files and the 
Adafruit SAMD board packages to get hold of the source code that I worked from. See footnote for path and hyperlink.
Arduino IDE version was 1.8.19, 


Only enough analog setup is done to allow compilation of SAMD51. 

At this point the tree is just a cutdown Adafruit SAMD arduino with microbian grafted on. 

The timer driver which was initially systick now makes use of an adaptation of the arduino Tone timer. The timer runs at 1ms
as was the case with arduino configured systick, it would probably make sense to adjust the timer to 2ms for
the 48MHz parts.

A warts and all history of brining up the trinket port can be seen in trinketm0-tinyusb git repository.

The trinketm0-tinyusb tree is slightly earlier than the combined tree. 
It has just enough work done to get a quick and dirty CDC port running as a microbian device.
As it is derived from Adafruit-TinyUsb library rather than tinyusb it incorporates C++ and currently pulls in lots of extra code.

I did my work on AMD64 Ubuntu 18.04.

To use:

export ADAFRUIT_BOARD=<BOARD>
where <BOARD> is one of "trinket_m0", "itsy_m0" or "itsy_m4" without quotes.

or for example 'make ADAFRUIT_BOARD=trinket_m0' from a project directory.

See "x00-blink", "x00-dotstar" or "x00-serial-driver" project directories.

Not all boards have the dotstar wired up so that a SERCOM could be used. Therefore the onboard
dotstar is bitbanged in every case.

Unlike the nrf51/52 microbian projects binaries are output to ../build/<BOARD>/<PROJECT>

Assuming export ADAFRUIT_BOARD=<BOARD>

'make flash' will program the device using bossac.

Or try 'make uf2 upload'

Mixing develoment of SAMD51 and SAMD21 boards may not be ideal for the paranoid as the bootloader sizes are
16K for *_m4 and 8K for *_m0. I think I saw somewhere that the boot area protect bit had been accidentaly left
in a dangerous state on some early board firmwares. 

I did all my development so far using bossac for flashing but added the uf2 and upload makefile rules for the
combined tree.

There is plenty to do in cleaning up what I have so far, but I have pulled eveything together before I pause 
development. 

If you want to fork the repository and improve things.

Footnote:
Please follow https://learn.adafruit.com/adafruit-trinket-m0-circuitpython-arduino/arduino-ide-setup instructions to
install Arduino IDE and board support packages.

After installing arduino ide and adafruit packages see ~/.arduino15/packages/adafruit/hardware/samd/1.7.10/ for
information and code, especially ~/.arduino15/packages/adafruit/hardware/samd/1.7.10/cores



