#all: kernel.bin
#all: $(BINDIR)/kernel.bin
#all: ../build/trinketm0/x00-blink/blink.o

TRINKET_TEST = `[ -d '/media/$(LOGNAME)/TRINKETBOOT' ] && echo "TRINKET"`
ITSY_M0_TEST = `[ -d '/media/$(LOGNAME)/ITSYBOOT' ] && echo "ITSYm0"`
ITSY_M4_TEST = `[ -d '/media/$(LOGNAME)/ITSYM4BOOT' ] && echo "ITSYm4"`


mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir := $(dir $(mkfile_path))

getproject = $(lastword $(subst /, ,$1))
PROJECT := $(call getproject,$(mkfile_dir))
#try $(CURDIR)

#ADAFRUIT_BOARD = trinketm0
#PROJECT := microbian
BINDIR = ../build/$(ADAFRUIT_BOARD)/$(PROJECT)
OSDIR = ../build/$(ADAFRUIT_BOARD)/microbian

VARIANT_PATH := ../variant-$(ADAFRUIT_BOARD)

#all: $(BINDIR)/kernel.elf #works
#all: ../build/trinketm0/x00-blink/blink.o #works


PORT=ttyACM0
BOSSAC=~/.arduino15/packages/adafruit/tools/bossac/1.8.0-48-gb176eee/bossac

include $(VARIANT_PATH)/config/bossac_offset.mk

CC = arm-none-eabi-gcc

#CPU = -mcpu=cortex-m0plus -mthumb #microbian uses cortex-m0 not plus
include $(VARIANT_PATH)/config/cpu.mk
#CFLAGS = -O -g -Wall -ffreestanding
#CRELEASEFLAGS = -O -Wall -ffreestanding
#CFLAGS=-g -nostartfiles -O0 -DF_CPU=48000000UL -mcpu=cortex-m0plus -mthumb

#CFLAGS=-g -nostartfiles -O0 -DF_CPU=48000000UL -mcpu=cortex-m0plus -mthumb \
#-D__SAMD21E18A__ -DCRYSTALLESS -DADAFRUIT_TRINKET_M0 -DARDUINO_SAMD_ZERO -DARM_MATH_CM0PLUS -DUSB_VID=0x239A -DUSB_PID=0x801e \
#-DUSBCON -DUSB_CONFIG_POWER=100 "-DUSB_MANUFACTURER=\"Adafruit\"" "-DUSB_PRODUCT=\"Trinket M0\"" -DUSE_TINYUSB

#remove -DUSE_TINYUSB for now
include $(VARIANT_PATH)/config/cflags.mk
#CFLAGS= -Wall -g -nostartfiles -O -DF_CPU=48000000UL -ffreestanding \
#-D__SAMD21E18A__ -DCRYSTALLESS -DADAFRUIT_TRINKET_M0 -DARDUINO_SAMD_ZERO -DARM_MATH_CM0PLUS -DUSB_VID=0x239A -DUSB_PID=0x801e \
#-DUSBCON -DUSB_CONFIG_POWER=100 "-DUSB_MANUFACTURER=\"Adafruit\"" "-DUSB_PRODUCT=\"Trinket M0\""

#LFLAGS=-Wl,-T,trinketm0.ld

#INCLUDE = -I ../microbian
#INCLUDE = -I ~/.arduino15/packages/adafruit/tools/CMSIS-Atmel/1.2.2/CMSIS/Device/ATMEL \
#          -I ~/.arduino15/packages/adafruit/tools/CMSIS-Atmel/1.2.2/CMSIS/Device/ATMEL/samd21 \
#          -I ~/.arduino15/packages/adafruit/tools/CMSIS/5.4.0/CMSIS/Core/Include \
#          -I $(VARIANT_PATH) \
#          -I ../microbian
include $(VARIANT_PATH)/config/include.mk

INCLUDE += -I $(VARIANT_PATH) \
	   -I ../microbian



AS = arm-none-eabi-as
LD = arm-none-eabi-ld
SIZE = arm-none-eabi-size
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
NM = arm-none-eabi-nm


vpath %.h ../microbian

all: $(BINDIR)/kernel.bin


$(BINDIR)/%.o: %.cpp
	$(CC) $(CPU) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(BINDIR)/%.o: %.c
	$(CC) $(CPU) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(BINDIR)/%.o: %.s
	$(AS) $(CPU) $< -o $@

APP = $(BINDIR)/blink.o $(BINDIR)/force_bootloader.o

WORKAROUND = $(OSDIR)/startup.o $(OSDIR)/microbian.a

#note cortex_handlers.c temp-wiring.c arduino-startup.c merged into startup.c
#less good for obviously showing copyright but all comments left intact inside startup.c
$(BINDIR)/%.elf: $(APP) $(WORKAROUND)
	$(CC) $(CPU) $(CFLAGS) -T $(VARIANT_PATH)/microbian_flash_with_bootloader.ld \
		$^ -nostdlib -lgcc -lc -o $@ -Wl,-Map,$*.map
	$(SIZE) $@

disasm:
	${OBJDUMP} -d $(BINDIR)/kernel.elf > kernel.elf.asm
	${NM} $(BINDIR)/kernel.elf > kernel.elf.nm
	hexdump $(BINDIR)/kernel.bin > kernel.bin.hexdump

$(BINDIR)/%.bin: $(BINDIR)/%.elf
	$(OBJCOPY) -O binary $< $@

$(OSDIR)/microbian.a:
	$(MAKE) -C ../microbian all ADAFRUIT_BOARD=$(ADAFRUIT_BOARD) # re-calculate VARIANT_PATH in submake
#	$(MAKE) -C $(@D) all

# Nuke the default rules for building executables
SORRY = echo "Please say 'make $@.hex' to compile '$@'"
%: %.s; @$(SORRY)
%: %.o; @$(SORRY)


clean:
	rm -f $(BINDIR)/*.bin $(BINDIR)/*.elf $(BINDIR)/*.o \
	*.nm *.map *elf.asm *.hexdump #2> /dev/null


ultraclean: clean
	rm -f $(OSDIR)/*.a $(OSDIR)/*.o

flash: kernel.bin
	${BOSSAC} -i -d --port=${PORT} -i -e -w -v kernel.bin -R --offset $(BOSSAC_OFFSET)

# Don't delete intermediate files
.SECONDARY:

###

$(BINDIR)/blink.o $(BINDIR)/force_bootloader.o: microbian.h lib.h hardware.h
