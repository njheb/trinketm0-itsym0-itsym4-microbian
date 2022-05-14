mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir := $(dir $(mkfile_path))

getproject = $(lastword $(subst /, ,$1))
project := $(call getproject,$(mkfile_dir))

VARIANT_PATH="FIX"
M0_OFFSET := 0x2000
M4_OFFSET := 0x4000

#BOSSAC_OFFSET=$(M0_OFFSET)

.PHONEY: flash clean

all: DO_$(ADAFRUIT_BOARD)
	@date
	@echo "tail"

#ifeq ( $(ADAFRUIT_BOARD), "TRINKETM0" )
#	VARIANT_PATH="variant-trinket_m0"
#else
#	ifeq ( $(ADAFRUIT_BOARD), "ITSYM0" )
#		VARIANT_PATH="variant-itsybitsy_m0"
#	else
#		ifeq ( $(ADAFRUIT_BOARD), "ITSYM4" )
#			VARIANT_PATH="variant-itsybitsy_m4"
#		else
#			@echo "NO \$ADAFRUIT_BOARD defined"
#		endif
#	endif
#endif


DO_trinket_m0:
	echo "Trinket_m0"
	make -f $(project).mk BOSSAC_OFFSET=$(M0_OFFSET) VARIANT_PATH="variant-trinket_m0" $(MAKECMDGOALS)

DO_itsy_m0:
	@echo "ItsyBitsy_m0"
	make -f $(project).mk BOSSAC_OFFSET=$(M0_OFFSET) VARIANT_PATH="variant-itsybitsy_m0" $(MAKECMDGOALS)

DO_itsy_m4:
	@echo "ItsyBitsy_m4"
	make -f $(project).mk BOSSAC_OFFSET=$(M4_OFFSET) VARIANT_PATH="variant-itsybitsy_m4" $(MAKECMDGOALS)

DO_:
	@echo "'export ADAFRUIT_BOARD=trinket_m0' for example and re-run make"
	@echo "alternatives are: trinket_m0, itsy_m0, itsy_m4"
#	@exit 1

test:
	echo $(BOSSAC_OFFSET)
	echo $(VARIANT_PATH)
	echo $(mkfile_path)
	echo $(mkfile_dir)
	echo $(MAKEFILE_LIST)
	echo $(thing)
	echo $(CURDIR)
	echo $(project)

flash:
	@true

clean:
	@true
