# microbian/Makefile
# Copyright (c) 2020 J. M. Spivey
#ADAFRUIT_BOARD := trinketm0
PROJECT := microbian
BINDIR := ../build/$(ADAFRUIT_BOARD)/$(PROJECT)

all: $(BINDIR)/microbian.a $(BINDIR)/startup.o

VARIANT_PATH = ../variant-$(ADAFRUIT_BOARD)

#CPU = -mcpu=cortex-m0plus -mthumb
include $(VARIANT_PATH)/config/cpu.mk


#CFLAGS = -Wall -g -nostartfiles -O -DF_CPU=48000000UL -ffreestanding \
#-D__SAMD21E18A__ -DCRYSTALLESS -DADAFRUIT_TRINKET_M0 -DARDUINO_SAMD_ZERO -DARM_MATH_CM0PLUS
include $(VARIANT_PATH)/config/cflags.mk


CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
AR = arm-none-eabi-ar

#INCLUDE = -I ~/.arduino15/packages/adafruit/tools/CMSIS-Atmel/1.2.2/CMSIS/Device/ATMEL \
#          -I ~/.arduino15/packages/adafruit/tools/CMSIS-Atmel/1.2.2/CMSIS/Device/ATMEL/samd21 \
#          -I ~/.arduino15/packages/adafruit/tools/CMSIS/5.4.0/CMSIS/Core/Include \

#INCLUDE = -I "bug or i don't understand"

include $(VARIANT_PATH)/config/include.mk

INCLUDE +=  -I $(VARIANT_PATH)

#DRIVERS = timer.o serial.o i2c.o radio.o display.o
DRIVERS = $(BINDIR)/timer.o $(BINDIR)/serial.o $(BINDIR)/onboard_dotstar.o

#BODGE = cortex_handlers.o delay.o temp-wiring.o arduino-startup.o polling-uart.o
#moved code from temp-wiring.c and arduino-startup.c into startup.c
#move cortex handler code to end of startup.c

#tick_timer.o provides setup for timer.o driver
BODGE = $(BINDIR)/polling-uart.o $(BINDIR)/delay.o $(BINDIR)/minimal-analog.o $(BINDIR)/tick_timer.o
#BODGE = polling-uart.o minimal-analog.o tick_timer.o

ifeq ($(ADAFRUIT_BOARD),itsy_m4)
	CONTEXT_ETC := mpx-m4.o
else
	CONTEXT_ETC := mpx-m0.o
endif

MICROBIAN = $(BINDIR)/microbian.o $(BINDIR)/$(CONTEXT_ETC) $(DRIVERS) $(BINDIR)/lib.o $(BODGE)

$(BINDIR)/microbian.a: $(MICROBIAN)
	$(AR) cr $@ $^

$(BINDIR)/%.o: %.c
	$(CC) $(CPU) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(BINDIR)/%.o: %.s
	$(AS) $(CPU) $< -o $@

clean: force
	rm -f $(BINDIR)/microbian.a $(BINDIR)/*.o

force:

###

$(MICROBIAN) $(BINDIR)/startup.o: microbian.h hardware.h lib.h
