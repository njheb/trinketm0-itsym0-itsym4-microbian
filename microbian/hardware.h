/* common/hardware.h */
/* Copyright (c) 2018-20 J. M. Spivey */

#define UBIT 1
#define UBIT_V1 1

/* Hardware register definitions for nRF51822 */

#define BIT(i) (1 << (i))
#define GET_BIT(reg, n) (((reg) >> (n)) & 0x1)
#define SET_BIT(reg, n) reg |= BIT(n)
#define CLR_BIT(reg, n) reg &= ~BIT(n)

#define GET_BYTE(reg, n) (((reg) >> (8*(n))) & 0xff)
#define SET_BYTE(reg, n, v) \
    reg = (reg & ~(0xff << 8*n)) | ((v & 0xff) << 8*n)

/* The macros SET_FIELD, etc., are defined in an indirect way that
permits (because of the timing of CPP macro expansion) the 'field'
argument to be a macro that expands the a 'position, width' pair. */

#define SET_FIELD(reg, field, val) __SET_FIELD(reg, field, val)
#define __SET_FIELD(reg, pos, wid, val) \
    reg = (reg & ~__MASK(pos, wid)) | __FIELD(pos, wid, val)

#define GET_FIELD(reg, field) __GET_FIELD(reg, field)
#define __GET_FIELD(reg, pos, wid)  ((reg >> pos) & __MASK0(wid))

#define FIELD(field, val) __FIELD(field, val)
#define __FIELD(pos, wid, val)  (((val) & __MASK0(wid)) << pos)

#define MASK(field) __MASK(field)
#define __MASK(pos, wid)  (__MASK0(wid) << pos)

#define __MASK0(wid)  (~((-2) << (wid-1)))

/* Interrupts */
#define SVC_IRQ    -5
#define PENDSV_IRQ -2

#if !defined(__SAMD51__)
#define PM_IRQ        0
#define SYSCTRL_IRQ   1
#define WD_IRQ        2
#define RTC_IRQ       3
#define EIC_IRQ       4
#define NVMCTRL_IRQ   5
#define DMAC_IRQ      6
#define USB_IRQ       7
#define EVSYS_IRQ     8
#define SERCOM0_IRQ   9
#define SERCOM1_IRQ   10
#define SERCOM2_IRQ   11
#define SERCOM3_IRQ   12
#define SERCOM4_IRQ   13
#define SERCOM5_IRQ   14
#define TCC0_IRQ      15
#define TCC1_IRQ      16
#define TCC2_IRQ      17
#define TC3_IRQ       18
#define TC4_IRQ       19
#define TC5_IRQ       20
#define TC6_IRQ       21
#define TC7_IRQ       22
#define ADC_IRQ       23
#define AC_IRQ        24
#define DAC_IRQ       25
#define PTC_IRQ       26
#define I2S_IRQ       27
#endif


//multiple irq sources per SERCOM unlike samd21 is most likely going to be problematic
#if defined( __SAMD51__ )
#define UART_IRQ 	SERCOM3_0_IRQn
#define UART_IRQ1	SERCOM3_1_IRQn
#define UART_IRQ2	SERCOM3_2_IRQn
#define UART_IRQ3	SERCOM3_3_IRQn
#else
#define UART_IRQ SERCOM0_IRQ
#endif

//SERCOM0_IRQn check value is 9
//#define I2C_IRQ     
//#define SPI_IRQ     

//#define TIMER0_IRQ  
//#define TIMER1_IRQ  
//#define TIMER2_IRQ  
//#define RTC0_IRQ ?number of instances

#if defined(__SAMD51__)
#define N_INTERRUPTS 138 /*CHECK************************CHECK****************************CHECK*/
#else
#define N_INTERRUPTS 32
#endif

/* Device register structures */
#define _DEVICE  union
#define _REGISTER(decl, offset) \
    struct { unsigned char __pad##offset[offset]; decl; }
/* System contol block */

_DEVICE _scb {
    _REGISTER(unsigned CPUID, 0x00);
    _REGISTER(unsigned ICSR, 0x04);
#define   SCB_ICSR_PENDSVSET 28
#define   SCB_ICSR_VECTACTIVE 0, 8
    _REGISTER(unsigned SCR, 0x10);
#define   SCB_SCR_SLEEPONEXIT 1
#define   SCB_SCR_SLEEPDEEP 2
#define   SCB_SCR_SEVONPEND 4
  /*  _REGISTER(unsigned SHPR[3], 0x18);*/ /*differs cm0 and cm0plus*/
};

#define M_SCB (* (volatile _DEVICE _scb *) 0xe000ed00)

#if 0
/* Nested vectored interupt controller */
_DEVICE _nvic {
    _REGISTER(unsigned ISER[8], 0x100);
    _REGISTER(unsigned ICER[8], 0x180);
    _REGISTER(unsigned ISPR[8], 0x200);
    _REGISTER(unsigned ICPR[8], 0x280);
    _REGISTER(unsigned IPR[60], 0x400);
};

//check address have datasheet for g need samd21e18a g and e seem equivalent
#define M_NVIC (* (volatile _DEVICE _nvic *) 0xe000e100)
#endif

#if 1 //tentative not checked
/* NVIC stuff */

/* irq_priority -- set priority of an IRQ from 0 (highest) to 255 */
//void irq_priority(int irq, unsigned priority); //check NVIC_SetPriority
#define irq_priority(irq, priority) NVIC_SetPriority(irq, priority)

/* enable_irq -- enable interrupts from an IRQ */
//#define enable_irq(irq)  NVIC.ISER[0] = BIT(irq)
//#define enable_irq(irq)  M_NVIC.ISER[0] = BIT(irq)
#define enable_irq(irq)    NVIC_EnableIRQ(irq)

/* disable_irq -- disable interrupts from a specific IRQ */
//#define disable_irq(irq)  NVIC.ICER[0] = BIT(irq)
//#define disable_irq(irq)  M_NVIC.ICER[0] = BIT(irq)
#define disable_irq(irq)    NVIC_DisableIRQ(irq)

/* clear_pending -- clear pending interrupt from an IRQ */
//#define clear_pending(irq)  NVIC.ICPR[0] = BIT(irq)
//#define clear_pending(irq)  M_NVIC.ICPR[0] = BIT(irq)
#define clear_pending(irq)  NVIC_ClearPendingIRQ(irq)

/* reschedule -- request PendSV interrupt */
//#define reschedule()  SCB.ICSR = BIT(SCB_ICSR_PENDSVSET)
//#define reschedule()  M_SCB.ICSR = BIT(SCB_ICSR_PENDSVSET) //28U is correct

#define reschedule() SCB->ICSR=SCB_ICSR_PENDSVSET_Msk
/*
SCB->ICSR = SCB_ICSR_PENDSVSET_Msk
~/.arduino15/packages/adafruit/tools/CMSIS/5.4.0/CMSIS/Core/Include/core_cm0plus.h
#define SCB_ICSR_PENDSVSET_Msk             (1UL << SCB_ICSR_PENDSVSET_Pos)                */


/*
exceptionNumber = (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk)
*/



/* active_irq -- find active interrupt: returns -16 to 31 */
//#define active_irq()  (GET_FIELD(SCB.ICSR, SCB_ICSR_VECTACTIVE) - 16)
/*
#define SCB_ICSR_VECTACTIVE_Pos             0U 
#define SCB_ICSR_VECTACTIVE_Msk            (0x1FFUL 

*/

/*info says NVIC_GetActive() not available on Cortex-M0+*/
//NVIC_GetActive() not the right function anyway
#if 1//!defined( __SAMD51__ )
#define active_irq()  (GET_FIELD(M_SCB.ICSR, SCB_ICSR_VECTACTIVE) - 16)
#else
#define active_irq()  ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) - 16) 
#endif

#endif
/* delay_loop -- timed delay */
void delay_loop(unsigned usec);

#if 0
/* GPIO convenience */

/* gpio_dir -- set GPIO direction */
inline void gpio_dir(unsigned pin, unsigned dir) {
    if (dir)
        GPIO.DIRSET = BIT(pin);
    else
        GPIO.DIRCLR = BIT(pin);
}

/* gpio_connect -- connect pin for input */
inline void gpio_connect(unsigned pin) {
    SET_FIELD(GPIO.PINCNF[pin], GPIO_PINCNF_INPUT, GPIO_INPUT_Connect);
}

/* gpio_drive -- set GPIO drive strength */
inline void gpio_drive(unsigned pin, unsigned mode) {
    SET_FIELD(GPIO.PINCNF[pin], GPIO_PINCNF_DRIVE, mode);
}

/* gpio_out -- set GPIO output value */
inline void gpio_out(unsigned pin, unsigned value) {
    if (value)
        GPIO.OUTSET = BIT(pin);
    else
        GPIO.OUTCLR = BIT(pin);
}

/* gpio_in -- get GPIO input bit */
inline unsigned gpio_in(unsigned pin) {
    return GET_BIT(GPIO.IN, pin);
}
#endif

#if 0
#define LED_MASK 0xfff0

#define led_init()  GPIO.DIRSET = LED_MASK
#define led_dot()   GPIO.OUTSET = 0x5fbf
#define led_off()   GPIO.OUTCLR = LED_MASK
#endif

/* CODERAM -- mark function for copying to RAM (disabled on V1) */
#define CODERAM

/* A few assembler macros for single instructions. */
#define pause()         asm volatile ("wfe")
#define intr_disable()  asm volatile ("cpsid i")
#define intr_enable()   asm volatile ("cpsie i")
#define get_primask()   ({unsigned x; asm ("mrs %0, primask" : "=r" (x)); x;})
#define set_primask(x)  asm ("msr primask, %0" : : "r" (x))
#define nop()           asm volatile ("nop")
#define syscall(op)     asm volatile ("svc %0" : : "i"(op))
