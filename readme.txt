Microbian nrf51/52 ported to Adafruit SAMD21/51 (early work)
============================================================

For the original BBC microbit version information please see: 
	https://spivey.oriel.ox.ac.uk/baremetal/Bare_metal_micro:bit
	https://github.com/Spivoxity/microbian
	https://spivey.oriel.ox.ac.uk/digisys/Lecture_notes

Supported Boards so far in this port: Trinket_m0, ItsyBitsy_m0, ItsyBitsy_m4

For overview of boards see:
	https://learn.adafruit.com/introducing-adafruit-itsybitsy-m4
	https://learn.adafruit.com/introducing-itsy-bitsy-m0
	https://learn.adafruit.com/adafruit-trinket-m0-circuitpython-arduino

pinout diagrams:
	https://github.com/adafruit/Adafruit-ItsyBitsy-M0-PCB/blob/master/Adafruit%20ItsyBitsy%20M0%20pinout.pdf
	https://github.com/adafruit/Adafruit-Trinket-M0-PCB/blob/master/Adafruit%20Trinket%20M0%20pinout.pdf
	ItsyBitsy_m4 appears not to have a comparable diagram.

datasheets:
	https://www.datasheets.com/en/part-details/atsamd21g18a-au-microchip-technology-60818854#datasheet
	https://www.microchip.com/sitesearch/search/All/ATSAMD51G19A

The Itsy boards have the SWD interface broken out onto header pins.

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

I did my work on AMD64 Ubuntu 18.04. This release has been tested with Raspberry pi 4 using pre-installed arm-none-eabi-* tools.
Please follow toolchain install instructions from https://spivey.oriel.ox.ac.uk/baremetal/Appendix_B:_Software_setup 

Watch out for gpio pin numbering, having divorced the code from it's arduino variant provided pin config struct. So far where 
necessary I have put in adhoc code to vary the port based on pin number where pins are assigned as n=(port*32)+gpio_bit. 
A proper scheme is needed.

To use: (for the combined tree)
=======

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

Footnote:
Please follow https://learn.adafruit.com/adafruit-trinket-m0-circuitpython-arduino/arduino-ide-setup  instructions 
to install Arduino IDE and board support packages.

After installing arduino ide and adafruit packages see ~/.arduino15/packages/adafruit/hardware/samd/1.7.10/ for
information and code, especially ~/.arduino15/packages/adafruit/hardware/samd/1.7.10/cores

For raspberry pi install
visit https://www.arduino.cc/en/software
pick Linux ARM 32 bits or 64bits from download options as required

once downloaded assuming 32bits arm download
tar xf arduino-1.8.19-linuxarm.tar.xz
cd arduino-1.8.19
sudo sh install.sh
arduino

"in File>preferences set Additional Boards Manager URLs to"
https://adafruit.github.io/arduino-board-index/package_adafruit_index.json

"go to Board manager, scroll down and find Adafruit SAMD Boards"
"select install 1.7.10"

After installation of boards it is suggested best practice to close and reopen arduino if you want to 
try out arduino examples before moving on to using the rudimentary SAMD port of microbian using Makefiles.

See "To use": above.
