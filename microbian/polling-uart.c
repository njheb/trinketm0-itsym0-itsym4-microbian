#include "SERCOM-cutdown.h"
//#define XSTR(x) STR(x)
//#define STR(x) #x


void SERCOM_initClockNVIC( void ); //remove warning without reordering
//from WVariant.h
typedef enum _EPioType
{
  PIO_NOT_A_PIN=-1,     /* Not under control of a peripheral. */
  PIO_EXTINT=0,         /* The pin is controlled by the associated signal of peripheral A. */
  PIO_ANALOG,           /* The pin is controlled by the associated signal of peripheral B. */
  PIO_SERCOM,           /* The pin is controlled by the associated signal of peripheral C. */
  PIO_SERCOM_ALT,       /* The pin is controlled by the associated signal of peripheral D. */
  PIO_TIMER,            /* The pin is controlled by the associated signal of peripheral E. */
  PIO_TIMER_ALT,        /* The pin is controlled by the associated signal of peripheral F. */
#if defined(__SAMD51__)
  PIO_TCC_PDEC,                 /* The pin is controlled by the associated signal of peripheral G. */
  PIO_COM,             /* The pin is controlled by the associated signal of peripheral H. */
  PIO_SDHC,             /* The pin is controlled by the associated signal of peripheral I. */
  PIO_I2S,              /* The pin is controlled by the associated signal of peripheral J. */
  PIO_PCC,              /* The pin is controlled by the associated signal of peripheral K. */
  PIO_GMAC,             /* The pin is controlled by the associated signal of peripheral L. */
  PIO_AC_CLK,           /* The pin is controlled by the associated signal of peripheral M. */
  PIO_CCL,              /* The pin is controlled by the associated signal of peripheral N. */
#else
  PIO_COM,              /* The pin is controlled by the associated signal of peripheral G. */
  PIO_AC_CLK,           /* The pin is controlled by the associated signal of peripheral H. */
#endif
  PIO_DIGITAL,          /* The pin is controlled by PORT. */
  PIO_INPUT,            /* The pin is controlled by PORT and is an input. */
  PIO_INPUT_PULLUP,     /* The pin is controlled by PORT and is an input with internal pull-up resistor enabled. */
  PIO_OUTPUT,           /* The pin is controlled by PORT and is an output. */

  PIO_PWM=PIO_TIMER,
  PIO_PWM_ALT=PIO_TIMER_ALT,
} EPioType ;

typedef enum _EPortType
{
  NOT_A_PORT=-1,
  PORTA=0,
  PORTB=1,
  PORTC=2,
  PORTD=3,
} EPortType ;

/* Generic Clock Multiplexer IDs */
#define GCM_DFLL48M_REF           (0x00U)
#define GCM_FDPLL96M_INPUT        (0x01U)
#define GCM_FDPLL96M_32K          (0x02U)
#define GCM_WDT                   (0x03U)
#define GCM_RTC                   (0x04U)
#define GCM_EIC                   (0x05U)
#define GCM_USB                   (0x06U)
#define GCM_EVSYS_CHANNEL_0       (0x07U)
#define GCM_EVSYS_CHANNEL_1       (0x08U)
#define GCM_EVSYS_CHANNEL_2       (0x09U)
#define GCM_EVSYS_CHANNEL_3       (0x0AU)
#define GCM_EVSYS_CHANNEL_4       (0x0BU)
#define GCM_EVSYS_CHANNEL_5       (0x0CU)
#define GCM_EVSYS_CHANNEL_6       (0x0DU)
#define GCM_EVSYS_CHANNEL_7       (0x0EU)
#define GCM_EVSYS_CHANNEL_8       (0x0FU)
#define GCM_EVSYS_CHANNEL_9       (0x10U)
#define GCM_EVSYS_CHANNEL_10      (0x11U)
#define GCM_EVSYS_CHANNEL_11      (0x12U)
#define GCM_SERCOMx_SLOW          (0x13U)
#define GCM_SERCOM0_CORE          (0x14U)
#define GCM_SERCOM1_CORE          (0x15U)
#define GCM_SERCOM2_CORE          (0x16U)
#define GCM_SERCOM3_CORE          (0x17U)
#define GCM_SERCOM4_CORE          (0x18U)
#define GCM_SERCOM5_CORE          (0x19U)
#define GCM_TCC0_TCC1             (0x1AU)
#define GCM_TCC2_TC3              (0x1BU)
#define GCM_TC4_TC5               (0x1CU)
#define GCM_TC6_TC7               (0x1DU)
#define GCM_ADC                   (0x1EU)
#define GCM_AC_DIG                (0x1FU)
#define GCM_AC_ANA                (0x20U)
#define GCM_DAC                   (0x21U)
#define GCM_PTC                   (0x22U)
#define GCM_I2S_0                 (0x23U)
#define GCM_I2S_1                 (0x24U)


#if 0
//pindescription from WVariant.h
typedef struct _PinDescription
{
  EPortType       ulPort ;
  uint32_t        ulPin ;
  EPioType        ulPinType ;
  uint32_t        ulPinAttribute ;
  EAnalogChannel  ulADCChannelNumber ; /* ADC Channel number in the SAM device $*/
  EPWMChannel     ulPWMChannel ;
  ETCChannel      ulTCChannel ;
  EExt_Interrupts ulExtInt ;
} PinDescription ;

  // D3, ADC, PWM, IRQ, UART RX, Captouch and general purpose pin
  { PORTA,  7, PIO_SERCOM_ALT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel7, PWM1_CH1, TCC1_CH1, EXTERNAL_INT_7 }, // TCC1/W$

  // D4, ADC, PWM, IRQ, UART TX, Captouch and general purpose pin
  { PORTA,  6, PIO_SERCOM_ALT, (PIN_ATTR_DIGITAL|PIN_ATTR_PWM|PIN_ATTR_TIMER), ADC_Channel6, PWM1_CH0, TCC1_CH0, EXTERNAL_INT_6 }, // TCC1/W$



//from variant.h
// Serial1 (sercom 0)
#define PIN_SERIAL1_RX       (3ul) // PA07
#define PAD_SERIAL1_RX       (SERCOM_RX_PAD_3)   //see SERCOM.h = 3 enum
#define PIN_SERIAL1_TX       (4ul) // PA06
#define PAD_SERIAL1_TX       (UART_TX_PAD_2)     //see SERCOM.h = 0x1ul enum


//from variant.cpp
SERCOM sercom0( SERCOM0 ) ;
Uart Serial1( &sercom0, PIN_SERIAL1_RX, PIN_SERIAL1_TX, PAD_SERIAL1_RX, PAD_SERIAL1_TX ) ;

//void SERCOM0_Handler()
//{
//  Serial1.IrqHandler();
//}
#endif

#include "mcu_pins.h"

#if defined(__SAMD51__)

SercomClockSource clockSource = SERCOM_CLOCK_SOURCE_48M; //this is a nasty bodge WATCH OUT, fixes corrupt serial
/*
  // A briefly-available but now deprecated feature had the SPI clock source
  // set via a compile-time setting (MAX_SPI)...problem was this affected
  // ALL SERCOMs, whereas some (anything read/write, e.g. SD cards) should
  // not exceed the standard 24 MHz setting.  Newer code, if it needs faster
  // write-only SPI (e.g. to screen), should override the SERCOM clock on a
  // per-peripheral basis.  Nonetheless, we check SERCOM_SPI_FREQ_REF here
  // (MAX_SPI * 2) to retain compatibility with any interim projects that
  // might have relied on the compile-time setting.  But please, don't.
 #if SERCOM_SPI_FREQ_REF == F_CPU       // F_CPU clock = GCLK0
  clockSource = SERCOM_CLOCK_SOURCE_FCPU;
 #elif SERCOM_SPI_FREQ_REF == 48000000  // 48 MHz clock = GCLK1 (standard)
  clockSource = SERCOM_CLOCK_SOURCE_48M;
 #elif SERCOM_SPI_FREQ_REF == 100000000 // 100 MHz clock = GCLK2
  clockSource = SERCOM_CLOCK_SOURCE_100M;
 #endif
*/
#endif // end __SAMD51__

#if defined( ADAFRUIT_ITSYBITSY_M4  )
#define PAD_SERIAL1_RX       (SERCOM_RX_PAD_1)
#define PAD_SERIAL1_TX       (UART_TX_PAD_0)
Sercom* sercom = SERCOM3;
#else
#define PAD_SERIAL1_RX       (SERCOM_RX_PAD_3)   //see SERCOM.h = 3 enum
#define PAD_SERIAL1_TX       (UART_TX_PAD_2)     //see SERCOM.h = 0x1ul enum
Sercom* sercom = SERCOM0;
#endif
//not dealing with __SAMD51_ in constructor, probably only SPI specific_
//from constructor equivalent

typedef int bool;
const int true = 1;
const int false = 0;
//SercomUartMode mode = ?
//SercomUartSampleRate sampleRate = ?
/* =========================
 * ===== Sercom UART
 * =========================
*/


void SERCOM_resetUART()
{
  // Start the Software Reset
  sercom->USART.CTRLA.bit.SWRST = 1 ;

  while ( sercom->USART.CTRLA.bit.SWRST || sercom->USART.SYNCBUSY.bit.SWRST )
  {
    // Wait for both bits Software Reset from CTRLA and SYNCBUSY coming back to 0
  }
}


void SERCOM_initUART(SercomUartMode mode, SercomUartSampleRate sampleRate, uint32_t baudrate)
{
  SERCOM_initClockNVIC();
  SERCOM_resetUART();

  //Setting the CTRLA register
  sercom->USART.CTRLA.reg = SERCOM_USART_CTRLA_MODE(mode) |
                            SERCOM_USART_CTRLA_SAMPR(sampleRate);

#if 0
//moving to call from serial driver
  //Setting the Interrupt register
  sercom->USART.INTENSET.reg = SERCOM_USART_INTENSET_RXC |  //Received complete
                               SERCOM_USART_INTENSET_ERROR; //All others errors
#else
/*
  sercom->USART.INTENSET.reg = SERCOM_USART_INTENSET_DRE |
				SERCOM_USART_INTENSET_RXC |  //Received complete
                               SERCOM_USART_INTENSET_ERROR; //All others errors
*/
//  sercom->USART.INTENSET.reg = SERCOM_USART_INTENSET_RXC   //Received complete
//                               ; //All others errors

#endif
  if ( mode == UART_INT_CLOCK )
  {
    uint16_t sampleRateValue;

    if (sampleRate == SAMPLE_RATE_x16) {
      sampleRateValue = 16;
    } else {
      sampleRateValue = 8;
    }

    // Asynchronous fractional mode (Table 24-2 in datasheet)
    //   BAUD = fref / (sampleRateValue * fbaud)
    // (multiply by 8, to calculate fractional piece)
#if defined(__SAMD51__)
    uint32_t baudTimes8 = (SERCOM_FREQ_REF * 8) / (sampleRateValue * baudrate);
#else
    uint32_t baudTimes8 = (SystemCoreClock * 8) / (sampleRateValue * baudrate);
#endif

    sercom->USART.BAUD.FRAC.FP   = (baudTimes8 % 8);
    sercom->USART.BAUD.FRAC.BAUD = (baudTimes8 / 8);
  }
}

void SERCOM_initFrame(SercomUartCharSize charSize, SercomDataOrder dataOrder, SercomParityMode parityMode, SercomNumberStopBit nbStopBits)
{
  //Setting the CTRLA register
  sercom->USART.CTRLA.reg |=
    SERCOM_USART_CTRLA_FORM((parityMode == SERCOM_NO_PARITY ? 0 : 1) ) |
    dataOrder << SERCOM_USART_CTRLA_DORD_Pos;

  //Setting the CTRLB register
  sercom->USART.CTRLB.reg |= SERCOM_USART_CTRLB_CHSIZE(charSize) |
    nbStopBits << SERCOM_USART_CTRLB_SBMODE_Pos |
    (parityMode == SERCOM_NO_PARITY ? 0 : parityMode) <<
      SERCOM_USART_CTRLB_PMODE_Pos; //If no parity use default value
}

void SERCOM_initPads(SercomUartTXPad txPad, SercomRXPad rxPad)
{
  //Setting the CTRLA register
  sercom->USART.CTRLA.reg |= SERCOM_USART_CTRLA_TXPO(txPad) |
                             SERCOM_USART_CTRLA_RXPO(rxPad);

  // Enable Transceiver and Receiver
  sercom->USART.CTRLB.reg |= SERCOM_USART_CTRLB_TXEN | SERCOM_USART_CTRLB_RXEN ;
}

void SERCOM_enableUART()
{
  //Setting  the enable bit to 1
  sercom->USART.CTRLA.bit.ENABLE = 0x1u;

  //Wait for then enable bit from SYNCBUSY is equal to 0;
  while(sercom->USART.SYNCBUSY.bit.ENABLE);
}

bool SERCOM_isDataRegisterEmptyUART()
{
  //DRE : Data Register Empty
  return sercom->USART.INTFLAG.bit.DRE;
}

void SERCOM_flushUART()
{
  // Skip checking transmission completion if data register is empty
  if(SERCOM_isDataRegisterEmptyUART())
    return;

  // Wait for transmission to complete
  while(!sercom->USART.INTFLAG.bit.TXC);
}

void SERCOM_clearStatusUART()
{
  //Reset (with 0) the STATUS register
  sercom->USART.STATUS.reg = SERCOM_USART_STATUS_RESETVALUE;
}

bool SERCOM_availableDataUART()
{
  //RXC : Receive Complete
  return sercom->USART.INTFLAG.bit.RXC;
}

bool SERCOM_isTXCUART()
{
  //TXC : Transmit Complete
  return sercom->USART.INTFLAG.bit.TXC;
}

bool SERCOM_isUARTError()
{
  return sercom->USART.INTFLAG.bit.ERROR;
}

void SERCOM_acknowledgeUARTError()
{
//TODO: investigate
//warning: assignment of read-only location 'sercom->USART.INTFLAG.bit.ERROR'
  sercom->USART.INTFLAG.bit.ERROR = 1;
}

bool SERCOM_isBufferOverflowErrorUART()
{
  //BUFOVF : Buffer Overflow
  return sercom->USART.STATUS.bit.BUFOVF;
}

bool SERCOM_isFrameErrorUART()
{
  //FERR : Frame Error
  return sercom->USART.STATUS.bit.FERR;
}

void SERCOM_clearFrameErrorUART()
{
  // clear FERR bit writing 1 status bit
  sercom->USART.STATUS.bit.FERR = 1;
}

bool SERCOM_isParityErrorUART()
{
  //PERR : Parity Error
  return sercom->USART.STATUS.bit.PERR;
}

uint8_t SERCOM_readDataUART()
{
  return sercom->USART.DATA.bit.DATA;
}

int SERCOM_writeDataUART(uint8_t data)
{
  // Wait for data register to be empty
  while(!SERCOM_isDataRegisterEmptyUART());

  //Put data into DATA register
  sercom->USART.DATA.reg = (uint16_t)data;
  return 1;
}

void BODGE_writeDataUART(uint8_t data)
{
  sercom->USART.DATA.reg = (uint16_t)data;
}

void SERCOM_enableDataRegisterEmptyInterruptUART()
{
  sercom->USART.INTENSET.reg = SERCOM_USART_INTENSET_DRE;
}

void SERCOM_disableDataRegisterEmptyInterruptUART()
{
  sercom->USART.INTENCLR.reg = SERCOM_USART_INTENCLR_DRE;
}


void BODGE_enableRelevantInterruptUART(void)
{
  //Setting the Interrupt register
  sercom->USART.INTENSET.reg =  SERCOM_USART_INTENSET_DRE |
				SERCOM_USART_INTENSET_RXC |  //Received complete
                               SERCOM_USART_INTENSET_ERROR; //All others errors
}

void BODGE_enableSubsetInterruptUART(void)
{
  //Setting the Interrupt register
//maybe consider TXC inplace of DRE
  sercom->USART.INTENSET.reg =  SERCOM_USART_INTENSET_TXC |
				SERCOM_USART_INTENSET_RXC ;  //Received complete
}

void BODGE_disableRelevantInterruptUART(void)
{
  //Setting the Interrupt register
  sercom->USART.INTENCLR.reg =  SERCOM_USART_INTENCLR_DRE |
				SERCOM_USART_INTENCLR_RXC |  //Received complete
                               SERCOM_USART_INTENCLR_ERROR; //All others errors
}

void BODGE_clearTXC(void)
{
   sercom->USART.INTFLAG.reg = SERCOM_USART_INTFLAG_TXC;
}

void BODGE_clearRXC(void)
{
   sercom->USART.INTFLAG.reg = SERCOM_USART_INTFLAG_RXC;
}

//more code available at the bottom of  SERCOM.cpp

//pick up code from Uart.cpp - does some pinmode stuff
//Uart Serial1( &sercom0, PIN_SERIAL1_RX, PIN_SERIAL1_TX, PAD_SERIAL1_RX, PAD_SERIAL1_TX ) ;


//uint8_t uc_pinRX;
//uint8_t uc_pinTX;
SercomRXPad uc_padRX = PAD_SERIAL1_RX;
SercomUartTXPad uc_padTX = PAD_SERIAL1_TX;


//uc_pinRX = PIN_SERIAL1_RX; //3ul check that uc_pinRX is never used, require val from LUT
//uc_pinTX = PIN_SERIAL1_TX; //4ul check that uc_pinTX is never used
//uc_padRX = PAD_SERIAL1_RX; 
//uc_padTX = PAD_SERIAL1_TX;

//mods not needed yet, but ready for another board variant using something other than PORTA
void pinPeripheralUART(uint32_t ulPin, EPioType ulPeripheral)
{
     int port = (ulPin & 0x20) >> 5; //allow for signaling port via pin number
     ulPin &= 0x1f;

     if ( ulPin & 1 ) // is pin odd?
      {
        uint32_t temp ;

        // Get whole current setup for both odd and even pins and remove odd one
        temp = (PORT->Group[port].PMUX[ulPin >> 1].reg) & PORT_PMUX_PMUXE( 0xF ) ;
        // Set new muxing
        PORT->Group[port].PMUX[ulPin >> 1].reg = temp|PORT_PMUX_PMUXO( ulPeripheral ) ;
        // Enable port mux
        PORT->Group[port].PINCFG[ulPin].reg |= PORT_PINCFG_PMUXEN | PORT_PINCFG_DRVSTR;
      }
      else // even pin
      {
        uint32_t temp ;

        temp = (PORT->Group[port].PMUX[ulPin >> 1].reg) & PORT_PMUX_PMUXO( 0xF ) ;
        PORT->Group[port].PMUX[ulPin >> 1].reg = temp|PORT_PMUX_PMUXE( ulPeripheral ) ;
        PORT->Group[port].PINCFG[ulPin].reg |= PORT_PINCFG_PMUXEN | PORT_PINCFG_DRVSTR ; // Enable port mux
      }
}

void Uart_begin(unsigned long baudrate) //config fixed at 8N1
{

//trinket m0 specific
//          rx=ulPin3->7  =ulPeripheral uses PIO_SERCOM_ALT 

 pinPeripheralUART(PIN_REAL_RX, UART_PIO);  //see wiring_private.[ch]
//            tx=ulPin4->6 uses PIO_SERCOM_ALT
 pinPeripheralUART(PIN_REAL_TX, UART_PIO);

  SERCOM_initUART(UART_INT_CLOCK, SAMPLE_RATE_x16, baudrate);
//  SERCOM_initFrame(extractCharSize(config), LSB_FIRST, extractParity(config), extractNbStopBit(config));
  SERCOM_initFrame(UART_CHAR_SIZE_8_BITS, LSB_FIRST, SERCOM_NO_PARITY, SERCOM_STOP_BIT_1);
  SERCOM_initPads(uc_padTX, uc_padRX);

  SERCOM_enableUART();
}

void Uart_end()
{
  SERCOM_resetUART();
 // rxBuffer.clear();
 // txBuffer.clear();
}

void Uart_flush()
{
//  while(txBuffer.available()); // wait until TX buffer is empty

  SERCOM_flushUART();
}

void Uart_write(const uint8_t data)
{
//  while (!SERCOM_isDataRegisterEmptyUART()){;}

  SERCOM_writeDataUART(data);
}


#if defined(__SAMD51__)

static const struct {
  Sercom   *sercomPtr;
  uint8_t   id_core;
  uint8_t   id_slow;
  IRQn_Type irq[4];
} sercomData[] = {
  { SERCOM0, SERCOM0_GCLK_ID_CORE, SERCOM0_GCLK_ID_SLOW,
    SERCOM0_0_IRQn, SERCOM0_1_IRQn, SERCOM0_2_IRQn, SERCOM0_3_IRQn },
  { SERCOM1, SERCOM1_GCLK_ID_CORE, SERCOM1_GCLK_ID_SLOW,
    SERCOM1_0_IRQn, SERCOM1_1_IRQn, SERCOM1_2_IRQn, SERCOM1_3_IRQn },
  { SERCOM2, SERCOM2_GCLK_ID_CORE, SERCOM2_GCLK_ID_SLOW,
    SERCOM2_0_IRQn, SERCOM2_1_IRQn, SERCOM2_2_IRQn, SERCOM2_3_IRQn },
  { SERCOM3, SERCOM3_GCLK_ID_CORE, SERCOM3_GCLK_ID_SLOW,
    SERCOM3_0_IRQn, SERCOM3_1_IRQn, SERCOM3_2_IRQn, SERCOM3_3_IRQn },
  { SERCOM4, SERCOM4_GCLK_ID_CORE, SERCOM4_GCLK_ID_SLOW,
    SERCOM4_0_IRQn, SERCOM4_1_IRQn, SERCOM4_2_IRQn, SERCOM4_3_IRQn },
  { SERCOM5, SERCOM5_GCLK_ID_CORE, SERCOM5_GCLK_ID_SLOW,
    SERCOM5_0_IRQn, SERCOM5_1_IRQn, SERCOM5_2_IRQn, SERCOM5_3_IRQn },
#if defined(SERCOM6)
  { SERCOM6, SERCOM6_GCLK_ID_CORE, SERCOM6_GCLK_ID_SLOW,
    SERCOM6_0_IRQn, SERCOM6_1_IRQn, SERCOM6_2_IRQn, SERCOM6_3_IRQn },
#endif
#if defined(SERCOM7)
  { SERCOM7, SERCOM7_GCLK_ID_CORE, SERCOM7_GCLK_ID_SLOW,
    SERCOM7_0_IRQn, SERCOM7_1_IRQn, SERCOM7_2_IRQn, SERCOM7_3_IRQn },
#endif
};

#else // end if SAMD51 (prob SAMD21)

static const struct {
  Sercom   *sercomPtr;
  uint8_t   clock;
  IRQn_Type irqn;
} sercomData[] = {
  {SERCOM0, GCM_SERCOM0_CORE, SERCOM0_IRQn},
  {SERCOM1, GCM_SERCOM1_CORE, SERCOM1_IRQn},
  {SERCOM2, GCM_SERCOM2_CORE, SERCOM2_IRQn},
  {SERCOM3, GCM_SERCOM3_CORE, SERCOM3_IRQn},
#if defined(SERCOM4)
  {SERCOM4, GCM_SERCOM4_CORE, SERCOM4_IRQn},
#endif
#if defined(SERCOM5)
  {SERCOM5, GCM_SERCOM5_CORE, SERCOM5_IRQn},
#endif
};

#endif // end !SAMD51

int8_t SERCOM_getSercomIndex(void) {
  for(uint8_t i=0; i<(sizeof(sercomData) / sizeof(sercomData[0])); i++) {
    if(sercom == sercomData[i].sercomPtr) return i;
  }
  return -1;
}

#if defined(__SAMD51__)

//TODO should this have an initial value?
//SercomClockSource clockSource;
int freqRef;
// This is currently for overriding an SPI SERCOM's clock source only --
// NOT for UART or WIRE SERCOMs, where it will have unintended consequences.
// It does not check.
// SERCOM clock source override is available only on SAMD51 (not 21).
// A dummy function for SAMD21 (compiles to nothing) is present in SERCOM.h
// so user code doesn't require a lot of conditional situations.
void SERCOM_setClockSource(int8_t idx, SercomClockSource src, bool core) {

  if(src == SERCOM_CLOCK_SOURCE_NO_CHANGE) return;

  uint8_t clk_id = core ? sercomData[idx].id_core : sercomData[idx].id_slow;

  GCLK->PCHCTRL[clk_id].bit.CHEN = 0;     // Disable timer
  while(GCLK->PCHCTRL[clk_id].bit.CHEN);  // Wait for disable

  if(core) clockSource = src; // Save SercomClockSource value

  // From cores/arduino/startup.c:
  // GCLK0 = F_CPU
  // GCLK1 = 48 MHz
  // GCLK2 = 100 MHz
  // GCLK3 = XOSC32K
  // GCLK4 = 12 MHz
  if(src == SERCOM_CLOCK_SOURCE_FCPU) {
    GCLK->PCHCTRL[clk_id].reg =
      GCLK_PCHCTRL_GEN_GCLK0_Val | (1 << GCLK_PCHCTRL_CHEN_Pos);
    if(core) freqRef = F_CPU; // Save clock frequency value
  } else if(src == SERCOM_CLOCK_SOURCE_48M) {
    GCLK->PCHCTRL[clk_id].reg =
      GCLK_PCHCTRL_GEN_GCLK1_Val | (1 << GCLK_PCHCTRL_CHEN_Pos);
    if(core) freqRef = 48000000;
  } else if(src == SERCOM_CLOCK_SOURCE_100M) {
    GCLK->PCHCTRL[clk_id].reg =
      GCLK_PCHCTRL_GEN_GCLK2_Val | (1 << GCLK_PCHCTRL_CHEN_Pos);
    if(core) freqRef = 100000000;
  } else if(src == SERCOM_CLOCK_SOURCE_32K) {
    GCLK->PCHCTRL[clk_id].reg =
      GCLK_PCHCTRL_GEN_GCLK3_Val | (1 << GCLK_PCHCTRL_CHEN_Pos);
    if(core) freqRef = 32768;
  } else if(src == SERCOM_CLOCK_SOURCE_12M) {
    GCLK->PCHCTRL[clk_id].reg =
      GCLK_PCHCTRL_GEN_GCLK4_Val | (1 << GCLK_PCHCTRL_CHEN_Pos);
    if(core) freqRef = 12000000;
  }

  while(!GCLK->PCHCTRL[clk_id].bit.CHEN); // Wait for clock enable
}
#endif

void SERCOM_initClockNVIC( void )
{
  int8_t idx = SERCOM_getSercomIndex();
  if(idx < 0) return; // We got a problem here

#if defined(__SAMD51__)

  for(uint8_t i=0; i<4; i++) {
    NVIC_ClearPendingIRQ(sercomData[idx].irq[i]);
//    NVIC_SetPriority(sercomData[idx].irq[i], SERCOM_NVIC_PRIORITY);
    NVIC_EnableIRQ(sercomData[idx].irq[i]);
  }

  // SPI DMA speed is dictated by the "slow clock" (I think...maybe) so
  // BOTH are set to the same clock source (clk_slow isn't sourced from
  // XOSC32K as in prior versions of SAMD core).
  // This might have power implications for sleep code.

  SERCOM_setClockSource(idx, clockSource, true);  // true  = core clock
  SERCOM_setClockSource(idx, clockSource, false); // false = slow clock

#else // end if SAMD51 (prob SAMD21)

  uint8_t   clockId = sercomData[idx].clock;
  IRQn_Type IdNvic  = sercomData[idx].irqn;

  // Setting NVIC
#if 1

  NVIC_ClearPendingIRQ(IdNvic);
//  NVIC_SetPriority(IdNvic, SERCOM_NVIC_PRIORITY);
  NVIC_EnableIRQ(IdNvic);
#endif
  // Setting clock
  GCLK->CLKCTRL.reg =
    GCLK_CLKCTRL_ID( clockId ) | // Generic Clock 0 (SERCOMx)
    GCLK_CLKCTRL_GEN_GCLK0     | // Generic Clock Generator 0 is source
    GCLK_CLKCTRL_CLKEN;

  while(GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY); // Wait for synchronization

#endif // end !SAMD51
}

#if 0
causes multiple def if serial driver is compiled in
/* print_buf -- output routine for use by printf */
void print_buf(char *buf, int n)
{
#if 0
    /* Using sendrec() here avoids a potential priority inversion:
       with separate send() and receive() calls, a lower-priority
       client process can block a reply from the device driver. */

    message m;
    m.ptr1 = buf;
    m.int2 = n;
    sendrec(SERIAL_TASK, PUTBUF, &m);
#else
    for(int i = 0; i<n; i++)
    {
	Uart_write(buf[i]);
    }
#endif
}
#endif


