#if defined( ADAFRUIT_TRINKET_M0 )
#define PIN_REAL_LED 10
#define PIN_REAL_RX 7
#define PIN_REAL_TX 6
#define UART_PIO PIO_SERCOM_ALT
#define PIN_REAL_DOTSTAR_DAT 0
#define PIN_REAL_DOTSTAR_CLK 1
#endif

#if defined( ADAFRUIT_ITSYBITSY_M0 )
#define PIN_REAL_LED 17
#define PIN_REAL_RX 11
#define PIN_REAL_TX 10
#define UART_PIO PIO_SERCOM
#define PIN_REAL_DOTSTAR_DAT 1
#define PIN_REAL_DOTSTAR_CLK 0
#endif

#if defined( ADAFRUIT_ITSYBITSY_M4 )
//16 SERCOM 3.1
//17 SERCOM 3.0
#define PIN_REAL_LED 22
#define PIN_REAL_RX 16
#define PIN_REAL_TX 17
#define UART_PIO PIO_SERCOM_ALT
//use +0  for PORTA
//use +32 for PORTB
//use +64 for PORTC should it exist
#define PIN_REAL_DOTSTAR_DAT (32+3)
#define PIN_REAL_DOTSTAR_CLK (32+2)
#endif

#if !defined( ADAFRUIT_ITSYBITSY_M4 ) && \
    !defined( ADAFRUIT_ITSYBITSY_M0 ) && \
    !defined( ADAFRUIT_TRINKET_M0 )
#error "unknown board or incorrect define"
#endif

//#if !defined( ADAFRUIT_TRINKET_M0 ) && 
//    !defined( ADAFRUIT_ITSYBITSY_M0 ) 
// defined ( ADAFRUIT_ITSYBITSY_M4 )
//#error "Pins not defined for board or board not defined"
//#endif
