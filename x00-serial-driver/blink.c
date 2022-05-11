
/*
 *
 * Physical address from SAMD21E18A datasheet
 *
 * */
//#define WITHOUT_SAM_H

#ifndef WITHOUT_SAM_H
#include "sam.h"
#include "mcu_pins.h"
#endif
#include "microbian.h"
//#include "lib.h"
#include <string.h>

#ifdef WITHOUT_SAM_H
#define GPIO_BASE 0x41004400
#define PORTDIRSET 0x41004408
#define PORTOUT 0x41004410
#define PORTCLR 0x41004414
#define LED_GPIO_BIT PIN_REAL_LED
#endif

static int A_TASK;
static int B_TASK;
#define PINGPONG 333


extern void force_bootloader(void);
int count = 100;
#ifndef WITHOUT_SAM_H
uint32_t port = 0;
const uint32_t pin = PIN_REAL_LED;
const uint32_t pinMask = (1ul << pin);
#endif

void test_taskA(int arg)
{
//   int client;
   message m;

   while (1) {
	receive(PINGPONG, &m);
//        client = m.sender;

//        Uart_write('X');
//        printf("X %d\n", count);
          printf("%d\n", count);
//        serial_putc('G');
//        serial_putc('H');
//        serial_putc('\n');

	/* Clear LED output pin*/
#ifdef WITHOUT_SAM_H
	*(unsigned int*)PORTCLR = (1 << LED_GPIO_BIT);
#else
        PORT->Group[port].OUTCLR.reg = pinMask;
#endif
   }
}

void test_taskB(int arg)
{
//   int client;
   message m;
   while (1) {
	receive(PINGPONG, &m);
//        client = m.sender;

//        Uart_write('Y');
        printf("Y %d\n", count);
		/* Set LED output pin*/
#ifdef WITHOUT_SAM_H
	*(unsigned int*)PORTOUT = (1 << LED_GPIO_BIT);
#else
        PORT->Group[port].OUTSET.reg = pinMask;
#endif

   }
}

void test_taskT(int arg)
{
   message m;
   timer_pulse(250);
   while (1) {
	receive(PING, NULL);
        count--;

        if (count <= 1) //10ms * 10000 = 100secs ish, check this works before adding to microbian port for first t$
        {
           Uart_end();
           force_bootloader();
        }
        if (count&1)
            send(A_TASK, PINGPONG, &m);
        else
            send(B_TASK, PINGPONG, &m);
   }
}

//see WVariant.h EPortType port = PORTA;
void init() {
	timer_init();
        serial_init();

//        Uart_begin(9600);
	/* Set output direction for LED pin*/
#ifdef WITHOUT_SAM_H
	* (unsigned int*)PORTDIRSET |= (1 << LED_GPIO_BIT);
#else
      // enable input, to support reading back values, with pullups disabled
      PORT->Group[port].PINCFG[pin].reg = (uint8_t) (PORT_PINCFG_INEN | PORT_PINCFG_DRVSTR);
      // Set pin to output mode
      PORT->Group[port].DIRSET.reg = pinMask;
#endif
  A_TASK = start("TestA", test_taskA, 0, STACK);
  B_TASK = start("TestB", test_taskB, 0, STACK);

           start("TestTimer", test_taskT, 0, STACK);
}
