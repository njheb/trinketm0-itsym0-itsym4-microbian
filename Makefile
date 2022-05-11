VARIANT_PATH="FIX"
M0_OFFSET := 0x2000
M4_OFFSET := 0x4000

#BOSSAC_OFFSET=$(M0_OFFSET)

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


DO_TRINKETM0:
	echo "Trinket_m0"
	make test BOSSAC_OFFSET=$(M0_OFFSET) VARIANT_PATH="T0"

DO_ITSYM0:
	@echo "ItsyBitsy_m0"
	make test BOSSAC_OFFSET=$(M0_OFFSET) VARIANT_PATH="I0"


DO_ITSYM4:
	@echo "ItsyBitsy_m4"
	make test BOSSAC_OFFSET=$(M4_OFFSET) VARIANT_PATH="I4"

DO_:
	@echo "'export ADAFRUIT_BOARD=TRINKETM0' for example and re-run make"
	@echo "alternatives are: TRINKETM0, ITSYM0, ITSYM4"
#	@exit 1

test:
	echo $(BOSSAC_OFFSET)
	echo $(VARIANT_PATH)

