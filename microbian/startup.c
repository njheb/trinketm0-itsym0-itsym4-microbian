/* common/startup.c */
/* Copyright (c) 2018 J. M. Spivey */

//njh TODO radical changes to hardware.h needed
#include "hardware.h"
#include "delay.h" //for inline delayMicroseconds()
#include "mcu_pins.h"

//arduino-startup.c housed SystemInit()   was hoping to leave intact for copyright simplicity
/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.
  SAMD51 support added by Adafruit - Copyright (c) 2018 Dean Miller for Adafruit Industries

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "sam.h"
//#include "variant.h"
//next two defines instead of variant
//#define VARIANT_MAINOSC		(32768ul)
//#define VARIANT_MCK	(F_CPU)
#include "variant-cutdown.h"

//#include <stdio.h>

// Constants for Clock generators
#define GENERIC_CLOCK_GENERATOR_MAIN      (0u)

#if defined(__SAMD51__)
#define GENERIC_CLOCK_GENERATOR_XOSC32K   (3u)
#define GENERIC_CLOCK_GENERATOR_48M		  (1u)
#define GENERIC_CLOCK_GENERATOR_48M_SYNC	GCLK_SYNCBUSY_GENCTRL1
#define GENERIC_CLOCK_GENERATOR_100M	  (2u)
#define GENERIC_CLOCK_GENERATOR_100M_SYNC	GCLK_SYNCBUSY_GENCTRL2
#define GENERIC_CLOCK_GENERATOR_12M       (4u)
#define GENERIC_CLOCK_GENERATOR_12M_SYNC   GCLK_SYNCBUSY_GENCTRL4

//USE DPLL0 for 120MHZ
#define MAIN_CLOCK_SOURCE				  GCLK_GENCTRL_SRC_DPLL0

#define GENERIC_CLOCK_GENERATOR_1M		  (5u)
//#define CRYSTALLESS

#else

#define GENERIC_CLOCK_GENERATOR_XOSC32K   (1u)
#endif

#define GENERIC_CLOCK_GENERATOR_OSC32K    (1u)
#define GENERIC_CLOCK_GENERATOR_OSCULP32K (2u) /* Initialized at reset for WDT */
#define GENERIC_CLOCK_GENERATOR_OSC8M     (3u)
// Constants for Clock multiplexers
#define GENERIC_CLOCK_MULTIPLEXER_DFLL48M (0u)

void SystemInit( void )
{

//***************** SAMD51 ************************//
#if defined(__SAMD51__)
  NVMCTRL->CTRLA.reg |= NVMCTRL_CTRLA_RWS(0);
  
  #ifndef CRYSTALLESS
  /* ----------------------------------------------------------------------------------------------
   * 1) Enable XOSC32K clock (External on-board 32.768Hz oscillator)
   */
  
  OSC32KCTRL->XOSC32K.reg = OSC32KCTRL_XOSC32K_ENABLE | OSC32KCTRL_XOSC32K_EN32K | OSC32KCTRL_XOSC32K_EN32K | OSC32KCTRL_XOSC32K_CGM_XT | OSC32KCTRL_XOSC32K_XTALEN;
  
  while( (OSC32KCTRL->STATUS.reg & OSC32KCTRL_STATUS_XOSC32KRDY) == 0 ){
    /* Wait for oscillator to be ready */
  }

  #endif //CRYSTALLESS

  //software reset
	
  GCLK->CTRLA.bit.SWRST = 1;
  while ( GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_SWRST ){
	  /* wait for reset to complete */
  }

  #ifndef CRYSTALLESS  
  /* ----------------------------------------------------------------------------------------------
   * 2) Put XOSC32K as source of Generic Clock Generator 3
   */
  GCLK->GENCTRL[GENERIC_CLOCK_GENERATOR_XOSC32K].reg = GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_XOSC32K) | //generic clock gen 3
    GCLK_GENCTRL_GENEN;
  #else
  /* ----------------------------------------------------------------------------------------------
   * 2) Put OSCULP32K as source of Generic Clock Generator 3
   */
  GCLK->GENCTRL[GENERIC_CLOCK_GENERATOR_XOSC32K].reg = GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_OSCULP32K) | GCLK_GENCTRL_GENEN; //generic clock gen 3
  #endif
  

  while ( GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL3 ){
    /* Wait for synchronization */
  }
  
  /* ----------------------------------------------------------------------------------------------
   * 3) Put OSCULP32K as source for Generic Clock Generator 0
   */
  GCLK->GENCTRL[0].reg = GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_OSCULP32K) | GCLK_GENCTRL_GENEN;
  
  /* ----------------------------------------------------------------------------------------------
   * 4) Enable DFLL48M clock
   */

  while ( GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL0 ){
    /* Wait for synchronization */
  }

  /* DFLL Configuration in Open Loop mode */
  
  OSCCTRL->DFLLCTRLA.reg = 0;
  //GCLK->PCHCTRL[OSCCTRL_GCLK_ID_DFLL48].reg = (1 << GCLK_PCHCTRL_CHEN_Pos) | GCLK_PCHCTRL_GEN(GCLK_PCHCTRL_GEN_GCLK3_Val);
  
  OSCCTRL->DFLLMUL.reg = OSCCTRL_DFLLMUL_CSTEP( 0x1 ) |
    OSCCTRL_DFLLMUL_FSTEP( 0x1 ) |
    OSCCTRL_DFLLMUL_MUL( 0 );
  
  while ( OSCCTRL->DFLLSYNC.reg & OSCCTRL_DFLLSYNC_DFLLMUL )
    {
      /* Wait for synchronization */
    }
  
  OSCCTRL->DFLLCTRLB.reg = 0;
  while ( OSCCTRL->DFLLSYNC.reg & OSCCTRL_DFLLSYNC_DFLLCTRLB )
    {
      /* Wait for synchronization */
    }
  
  OSCCTRL->DFLLCTRLA.reg |= OSCCTRL_DFLLCTRLA_ENABLE;
  while ( OSCCTRL->DFLLSYNC.reg & OSCCTRL_DFLLSYNC_ENABLE )
    {
      /* Wait for synchronization */
    }
  
  OSCCTRL->DFLLVAL.reg = OSCCTRL->DFLLVAL.reg;
  while( OSCCTRL->DFLLSYNC.bit.DFLLVAL );
  
  OSCCTRL->DFLLCTRLB.reg = OSCCTRL_DFLLCTRLB_WAITLOCK |
  OSCCTRL_DFLLCTRLB_CCDIS | OSCCTRL_DFLLCTRLB_USBCRM ;
  
  while ( !OSCCTRL->STATUS.bit.DFLLRDY )
    {
      /* Wait for synchronization */
    }
  
  GCLK->GENCTRL[GENERIC_CLOCK_GENERATOR_1M].reg = GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_DFLL_Val) | GCLK_GENCTRL_GENEN | GCLK_GENCTRL_DIV(48u);
  
  while ( GCLK->SYNCBUSY.bit.GENCTRL5 ){
    /* Wait for synchronization */
  }
  
	  
  /* ------------------------------------------------------------------------
  * Set up the PLLs
  */
	
  //PLL0 is 120MHz
  GCLK->PCHCTRL[OSCCTRL_GCLK_ID_FDPLL0].reg = (1 << GCLK_PCHCTRL_CHEN_Pos) | GCLK_PCHCTRL_GEN(GCLK_PCHCTRL_GEN_GCLK5_Val);
  
  // This rounds to nearest full-MHz increment; not currently using frac
  OSCCTRL->Dpll[0].DPLLRATIO.reg = OSCCTRL_DPLLRATIO_LDRFRAC(0x00) | OSCCTRL_DPLLRATIO_LDR((F_CPU - 500000) / 1000000);
  
  while(OSCCTRL->Dpll[0].DPLLSYNCBUSY.bit.DPLLRATIO);
  
  //MUST USE LBYPASS DUE TO BUG IN REV A OF SAMD51
  OSCCTRL->Dpll[0].DPLLCTRLB.reg = OSCCTRL_DPLLCTRLB_REFCLK_GCLK | OSCCTRL_DPLLCTRLB_LBYPASS;
  
  OSCCTRL->Dpll[0].DPLLCTRLA.reg = OSCCTRL_DPLLCTRLA_ENABLE;
  
  while( OSCCTRL->Dpll[0].DPLLSTATUS.bit.CLKRDY == 0 || OSCCTRL->Dpll[0].DPLLSTATUS.bit.LOCK == 0 );
  
  //PLL1 is 100MHz
  GCLK->PCHCTRL[OSCCTRL_GCLK_ID_FDPLL1].reg = (1 << GCLK_PCHCTRL_CHEN_Pos) | GCLK_PCHCTRL_GEN(GCLK_PCHCTRL_GEN_GCLK5_Val);
  
  OSCCTRL->Dpll[1].DPLLRATIO.reg = OSCCTRL_DPLLRATIO_LDRFRAC(0x00) | OSCCTRL_DPLLRATIO_LDR(99); //100 Mhz
  
  while(OSCCTRL->Dpll[1].DPLLSYNCBUSY.bit.DPLLRATIO);
  
  //MUST USE LBYPASS DUE TO BUG IN REV A OF SAMD51
  OSCCTRL->Dpll[1].DPLLCTRLB.reg = OSCCTRL_DPLLCTRLB_REFCLK_GCLK | OSCCTRL_DPLLCTRLB_LBYPASS;
  
  OSCCTRL->Dpll[1].DPLLCTRLA.reg = OSCCTRL_DPLLCTRLA_ENABLE;
  
  while( OSCCTRL->Dpll[1].DPLLSTATUS.bit.CLKRDY == 0 || OSCCTRL->Dpll[1].DPLLSTATUS.bit.LOCK == 0 );
  
  
  /* ------------------------------------------------------------------------
  * Set up the peripheral clocks
  */
  
  //48MHZ CLOCK FOR USB AND STUFF
  GCLK->GENCTRL[GENERIC_CLOCK_GENERATOR_48M].reg = GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_DFLL_Val) |
    GCLK_GENCTRL_IDC |
    //GCLK_GENCTRL_OE |
    GCLK_GENCTRL_GENEN;
  
  while ( GCLK->SYNCBUSY.reg & GENERIC_CLOCK_GENERATOR_48M_SYNC)
    {
      /* Wait for synchronization */
    }
  
  //100MHZ CLOCK FOR OTHER PERIPHERALS
  GCLK->GENCTRL[GENERIC_CLOCK_GENERATOR_100M].reg = GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_DPLL1_Val) |
    GCLK_GENCTRL_IDC |
    //GCLK_GENCTRL_OE |
    GCLK_GENCTRL_GENEN;
  
  while ( GCLK->SYNCBUSY.reg & GENERIC_CLOCK_GENERATOR_100M_SYNC)
    {
      /* Wait for synchronization */
    }
  
  //12MHZ CLOCK FOR DAC
  GCLK->GENCTRL[GENERIC_CLOCK_GENERATOR_12M].reg = GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_DFLL_Val) |
    GCLK_GENCTRL_IDC |
    GCLK_GENCTRL_DIV(4) |
    //GCLK_GENCTRL_DIVSEL |
    //GCLK_GENCTRL_OE |
    GCLK_GENCTRL_GENEN;
  
  while ( GCLK->SYNCBUSY.reg & GENERIC_CLOCK_GENERATOR_12M_SYNC)
    {
      /* Wait for synchronization */
    }
  
  /*---------------------------------------------------------------------
   * Set up main clock
   */
  
  GCLK->GENCTRL[GENERIC_CLOCK_GENERATOR_MAIN].reg = GCLK_GENCTRL_SRC(MAIN_CLOCK_SOURCE) |
    GCLK_GENCTRL_IDC |
    //GCLK_GENCTRL_OE |
    GCLK_GENCTRL_GENEN;
  

  while ( GCLK->SYNCBUSY.reg & GCLK_SYNCBUSY_GENCTRL0 )
    {
      /* Wait for synchronization */
    }
  
  MCLK->CPUDIV.reg = MCLK_CPUDIV_DIV_DIV1;
  
  /* Use the LDO regulator by default */
  SUPC->VREG.bit.SEL = 0; 
  
  
  /* If desired, enable cache! */
#if defined(ENABLE_CACHE)
  __disable_irq();
  CMCC->CTRL.reg = 1;
  __enable_irq();
#endif

  /*---------------------------------------------------------------------
   * Start up the "Debug Watchpoint and Trace" unit, so that we can use
   * it's 32bit cycle counter for timing.
   */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

  /* ----------------------------------------------------------------------------------------------
   * 5) Load AC factory calibration values
   */

  uint32_t bias0 = (*((uint32_t *)AC_FUSES_BIAS0_ADDR) & AC_FUSES_BIAS0_Msk) >> AC_FUSES_BIAS0_Pos;
  AC->CALIB.reg = AC_CALIB_BIAS0(bias0);

  /* ----------------------------------------------------------------------------------------------
   * 6) Load ADC factory calibration values
   */

  // ADC0 Bias Calibration
  uint32_t biascomp = (*((uint32_t *)ADC0_FUSES_BIASCOMP_ADDR) & ADC0_FUSES_BIASCOMP_Msk) >> ADC0_FUSES_BIASCOMP_Pos;
  uint32_t biasr2r = (*((uint32_t *)ADC0_FUSES_BIASR2R_ADDR) & ADC0_FUSES_BIASR2R_Msk) >> ADC0_FUSES_BIASR2R_Pos;
  uint32_t biasref = (*((uint32_t *)ADC0_FUSES_BIASREFBUF_ADDR) & ADC0_FUSES_BIASREFBUF_Msk) >> ADC0_FUSES_BIASREFBUF_Pos;

  ADC0->CALIB.reg = ADC_CALIB_BIASREFBUF(biasref)
                    | ADC_CALIB_BIASR2R(biasr2r)
                    | ADC_CALIB_BIASCOMP(biascomp);

  // ADC1 Bias Calibration
  biascomp = (*((uint32_t *)ADC1_FUSES_BIASCOMP_ADDR) & ADC1_FUSES_BIASCOMP_Msk) >> ADC1_FUSES_BIASCOMP_Pos;
  biasr2r = (*((uint32_t *)ADC1_FUSES_BIASR2R_ADDR) & ADC1_FUSES_BIASR2R_Msk) >> ADC1_FUSES_BIASR2R_Pos;
  biasref = (*((uint32_t *)ADC1_FUSES_BIASREFBUF_ADDR) & ADC1_FUSES_BIASREFBUF_Msk) >> ADC1_FUSES_BIASREFBUF_Pos;

  ADC1->CALIB.reg = ADC_CALIB_BIASREFBUF(biasref)
                    | ADC_CALIB_BIASR2R(biasr2r)
                    | ADC_CALIB_BIASCOMP(biascomp);

  /* ----------------------------------------------------------------------------------------------
   * 7) Load USB factory calibration values
   */

  //USB Calibration
  uint32_t usbtransn = (*((uint32_t *)USB_FUSES_TRANSN_ADDR) & USB_FUSES_TRANSN_Msk) >> USB_FUSES_TRANSN_Pos;
  uint32_t usbtransp = (*((uint32_t *)USB_FUSES_TRANSP_ADDR) & USB_FUSES_TRANSP_Msk) >> USB_FUSES_TRANSP_Pos;
  uint32_t usbtrim = (*((uint32_t *)USB_FUSES_TRIM_ADDR) & USB_FUSES_TRIM_Msk) >> USB_FUSES_TRIM_Pos;
  USB->DEVICE.PADCAL.reg = USB_PADCAL_TRIM(usbtrim)
                           | USB_PADCAL_TRANSN(usbtransn)
                           | USB_PADCAL_TRANSP(usbtransp);

//*************** END SAMD51 *************************//
  
#else
//********************** SAMD21 *********************//

	/**
 * \brief SystemInit() configures the needed clocks and according Flash Read Wait States.
 * At reset:
 * - OSC8M clock source is enabled with a divider by 8 (1MHz).
 * - Generic Clock Generator 0 (GCLKMAIN) is using OSC8M as source.
 * We need to:
 * 1) Enable XOSC32K clock (External on-board 32.768Hz oscillator), will be used as DFLL48M reference.
 * 2) Put XOSC32K as source of Generic Clock Generator 1
 * 3) Put Generic Clock Generator 1 as source for Generic Clock Multiplexer 0 (DFLL48M reference)
 * 4) Enable DFLL48M clock
 * 5) Switch Generic Clock Generator 0 to DFLL48M. CPU will run at 48MHz.
 * 6) Modify PRESCaler value of OSCM to have 8MHz
 * 7) Put OSC8M as source for Generic Clock Generator 3
 */

	
  /* Set 1 Flash Wait State for 48MHz, cf tables 20.9 and 35.27 in SAMD21 Datasheet */
    NVMCTRL->CTRLB.bit.RWS = NVMCTRL_CTRLB_RWS_HALF_Val ;

    /* Turn on the digital interface clock */
    PM->APBAMASK.reg |= PM_APBAMASK_GCLK ;


  #if defined(CRYSTALLESS)

    /* ----------------------------------------------------------------------------------------------
     * 1) Enable OSC32K clock (Internal 32.768Hz oscillator)
     */

    uint32_t calib = (*((uint32_t *) FUSES_OSC32K_CAL_ADDR) & FUSES_OSC32K_CAL_Msk) >> FUSES_OSC32K_CAL_Pos;

    SYSCTRL->OSC32K.reg = SYSCTRL_OSC32K_CALIB(calib) |
                          SYSCTRL_OSC32K_STARTUP( 0x6u ) | // cf table 15.10 of product datasheet in chapter 15.8.6
                          SYSCTRL_OSC32K_EN32K |
                          SYSCTRL_OSC32K_ENABLE;

    while ( (SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_OSC32KRDY) == 0 ); // Wait for oscillator stabilization

  #else // has crystal

    /* ----------------------------------------------------------------------------------------------
     * 1) Enable XOSC32K clock (External on-board 32.768Hz oscillator)
     */
    SYSCTRL->XOSC32K.reg = SYSCTRL_XOSC32K_STARTUP( 0x6u ) | /* cf table 15.10 of product datasheet in chapter 15.8.6 */
                           SYSCTRL_XOSC32K_XTALEN | SYSCTRL_XOSC32K_EN32K ;
    SYSCTRL->XOSC32K.bit.ENABLE = 1 ; /* separate call, as described in chapter 15.6.3 */

    while ( (SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_XOSC32KRDY) == 0 )
    {
      /* Wait for oscillator stabilization */
    }

  #endif

    /* Software reset the module to ensure it is re-initialized correctly */
    /* Note: Due to synchronization, there is a delay from writing CTRL.SWRST until the reset is complete.
     * CTRL.SWRST and STATUS.SYNCBUSY will both be cleared when the reset is complete, as described in chapter 13.8.1
     */
    GCLK->CTRL.reg = GCLK_CTRL_SWRST ;

    while ( (GCLK->CTRL.reg & GCLK_CTRL_SWRST) && (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) )
    {
      /* Wait for reset to complete */
    }

    /* ----------------------------------------------------------------------------------------------
     * 2) Put XOSC32K as source of Generic Clock Generator 1
     */
    GCLK->GENDIV.reg = GCLK_GENDIV_ID( GENERIC_CLOCK_GENERATOR_XOSC32K ) ; // Generic Clock Generator 1

    while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY )
    {
      /* Wait for synchronization */
    }

    /* Write Generic Clock Generator 1 configuration */
    GCLK->GENCTRL.reg = GCLK_GENCTRL_ID( GENERIC_CLOCK_GENERATOR_OSC32K ) | // Generic Clock Generator 1
  #if defined(CRYSTALLESS)
                        GCLK_GENCTRL_SRC_OSC32K | // Selected source is Internal 32KHz Oscillator
  #else
                        GCLK_GENCTRL_SRC_XOSC32K | // Selected source is External 32KHz Oscillator
  #endif
  //                      GCLK_GENCTRL_OE | // Output clock to a pin for tests
                        GCLK_GENCTRL_GENEN ;

    while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY )
    {
      /* Wait for synchronization */
    }

    /* ----------------------------------------------------------------------------------------------
     * 3) Put Generic Clock Generator 1 as source for Generic Clock Multiplexer 0 (DFLL48M reference)
     */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID( GENERIC_CLOCK_MULTIPLEXER_DFLL48M ) | // Generic Clock Multiplexer 0
                        GCLK_CLKCTRL_GEN_GCLK1 | // Generic Clock Generator 1 is source
                        GCLK_CLKCTRL_CLKEN ;

    while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY )
    {
      /* Wait for synchronization */
    }

    /* ----------------------------------------------------------------------------------------------
     * 4) Enable DFLL48M clock
     */

    /* DFLL Configuration in Closed Loop mode, cf product datasheet chapter 15.6.7.1 - Closed-Loop Operation */

    /* Remove the OnDemand mode, Bug http://avr32.icgroup.norway.atmel.com/bugzilla/show_bug.cgi?id=9905 */
    SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE;

    while ( (SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY) == 0 )
    {
      /* Wait for synchronization */
    }

    SYSCTRL->DFLLMUL.reg = SYSCTRL_DFLLMUL_CSTEP( 31 ) | // Coarse step is 31, half of the max value
                           SYSCTRL_DFLLMUL_FSTEP( 511 ) | // Fine step is 511, half of the max value
                           SYSCTRL_DFLLMUL_MUL( (VARIANT_MCK + VARIANT_MAINOSC/2) / VARIANT_MAINOSC ) ; // External 32KHz is the reference

    while ( (SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY) == 0 )
    {
      /* Wait for synchronization */
    }

  #if defined(CRYSTALLESS)

    #define NVM_SW_CALIB_DFLL48M_COARSE_VAL 58

    // Turn on DFLL
    uint32_t coarse =( *((uint32_t *)(NVMCTRL_OTP4) + (NVM_SW_CALIB_DFLL48M_COARSE_VAL / 32)) >> (NVM_SW_CALIB_DFLL48M_COARSE_VAL % 32) )
                     & ((1 << 6) - 1);
    if (coarse == 0x3f) {
      coarse = 0x1f;
    }
    // TODO(tannewt): Load this value from memory we've written previously. There
    // isn't a value from the Atmel factory.
    uint32_t fine = 0x1ff;

    SYSCTRL->DFLLVAL.bit.COARSE = coarse;
    SYSCTRL->DFLLVAL.bit.FINE = fine;
    /* Write full configuration to DFLL control register */
    SYSCTRL->DFLLMUL.reg = SYSCTRL_DFLLMUL_CSTEP( 0x1f / 4 ) | // Coarse step is 31, half of the max value
                           SYSCTRL_DFLLMUL_FSTEP( 10 ) |
                           SYSCTRL_DFLLMUL_MUL( (48000) ) ;

    SYSCTRL->DFLLCTRL.reg = 0;

    while ( (SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY) == 0 )
    {
      /* Wait for synchronization */
    }

    SYSCTRL->DFLLCTRL.reg =  SYSCTRL_DFLLCTRL_MODE |
                             SYSCTRL_DFLLCTRL_CCDIS |
                             SYSCTRL_DFLLCTRL_USBCRM | /* USB correction */
                             SYSCTRL_DFLLCTRL_BPLCKC;

    while ( (SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY) == 0 )
    {
      /* Wait for synchronization */
    }

    /* Enable the DFLL */
    SYSCTRL->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_ENABLE ;

  #else   // has crystal

    /* Write full configuration to DFLL control register */
    SYSCTRL->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_MODE | /* Enable the closed loop mode */
                             SYSCTRL_DFLLCTRL_WAITLOCK |
                             SYSCTRL_DFLLCTRL_QLDIS ; /* Disable Quick lock */

    while ( (SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY) == 0 )
    {
      /* Wait for synchronization */
    }

    /* Enable the DFLL */
    SYSCTRL->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_ENABLE ;

    while ( (SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLLCKC) == 0 ||
            (SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLLCKF) == 0 )
    {
      /* Wait for locks flags */
    }

  #endif

    while ( (SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY) == 0 )
    {
      /* Wait for synchronization */
    }

    /* ----------------------------------------------------------------------------------------------
     * 5) Switch Generic Clock Generator 0 to DFLL48M. CPU will run at 48MHz.
     */
    GCLK->GENDIV.reg = GCLK_GENDIV_ID( GENERIC_CLOCK_GENERATOR_MAIN ) ; // Generic Clock Generator 0

    while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY )
    {
      /* Wait for synchronization */
    }

    /* Write Generic Clock Generator 0 configuration */
    GCLK->GENCTRL.reg = GCLK_GENCTRL_ID( GENERIC_CLOCK_GENERATOR_MAIN ) | // Generic Clock Generator 0
                        GCLK_GENCTRL_SRC_DFLL48M | // Selected source is DFLL 48MHz
  //                      GCLK_GENCTRL_OE | // Output clock to a pin for tests
                        GCLK_GENCTRL_IDC | // Set 50/50 duty cycle
                        GCLK_GENCTRL_GENEN ;

    while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY )
    {
      /* Wait for synchronization */
    }

    /* ----------------------------------------------------------------------------------------------
     * 6) Modify PRESCaler value of OSC8M to have 8MHz
     */
    SYSCTRL->OSC8M.bit.PRESC = SYSCTRL_OSC8M_PRESC_0_Val ;  //CMSIS 4.5 changed the prescaler defines
    SYSCTRL->OSC8M.bit.ONDEMAND = 0 ;

    /* ----------------------------------------------------------------------------------------------
     * 7) Put OSC8M as source for Generic Clock Generator 3
     */
    GCLK->GENDIV.reg = GCLK_GENDIV_ID( GENERIC_CLOCK_GENERATOR_OSC8M ) ; // Generic Clock Generator 3

    /* Write Generic Clock Generator 3 configuration */
    GCLK->GENCTRL.reg = GCLK_GENCTRL_ID( GENERIC_CLOCK_GENERATOR_OSC8M ) | // Generic Clock Generator 3
                        GCLK_GENCTRL_SRC_OSC8M | // Selected source is RC OSC 8MHz (already enabled at reset)
  //                      GCLK_GENCTRL_OE | // Output clock to a pin for tests
                        GCLK_GENCTRL_GENEN ;

    while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY )
    {
      /* Wait for synchronization */
    }

    /*
     * Now that all system clocks are configured, we can set CPU and APBx BUS clocks.
     * There values are normally the one present after Reset.
     */
    PM->CPUSEL.reg  = PM_CPUSEL_CPUDIV_DIV1 ;
    PM->APBASEL.reg = PM_APBASEL_APBADIV_DIV1_Val ;
    PM->APBBSEL.reg = PM_APBBSEL_APBBDIV_DIV1_Val ;
    PM->APBCSEL.reg = PM_APBCSEL_APBCDIV_DIV1_Val ;

    SystemCoreClock=VARIANT_MCK ;

    /* ----------------------------------------------------------------------------------------------
     * 8) Load ADC factory calibration values
     */

    // ADC Bias Calibration
    uint32_t bias = (*((uint32_t *) ADC_FUSES_BIASCAL_ADDR) & ADC_FUSES_BIASCAL_Msk) >> ADC_FUSES_BIASCAL_Pos;

    // ADC Linearity bits 4:0
    uint32_t linearity = (*((uint32_t *) ADC_FUSES_LINEARITY_0_ADDR) & ADC_FUSES_LINEARITY_0_Msk) >> ADC_FUSES_LINEARITY_0_Pos;

    // ADC Linearity bits 7:5
    linearity |= ((*((uint32_t *) ADC_FUSES_LINEARITY_1_ADDR) & ADC_FUSES_LINEARITY_1_Msk) >> ADC_FUSES_LINEARITY_1_Pos) << 5;

    ADC->CALIB.reg = ADC_CALIB_BIAS_CAL(bias) | ADC_CALIB_LINEARITY_CAL(linearity);

    /*
     * 9) Disable automatic NVM write operations
     */
    NVMCTRL->CTRLB.bit.MANW = 1;
  #endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//temp-wiring.c     housed arduino_init()
/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.
  SAMD51 support added by Adafruit - Copyright (c) 2018 Dean Miller for Adafruit Industries

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

//#include "Arduino.h"
#include "sam.h"
#include "wiring_analog.h" //for AR_DEFAULT
#include "WVariant.h"      //for GCM_DAC

#ifdef __cplusplus
extern "C" {
#endif


#if defined(__SAMD51__)
uint32_t SystemCoreClock=F_CPU;
#else
/*
 * System Core Clock is at 1MHz (8MHz/8) at Reset.
 * It is switched to 48MHz in the Reset Handler (startup.c)
 */
uint32_t SystemCoreClock=1000000ul ;
#endif

/*
void calibrateADC()
{
  volatile uint32_t valeur = 0;

  for(int i = 0; i < 5; ++i)
  {
    ADC->SWTRIG.bit.START = 1;
    while( ADC->INTFLAG.bit.RESRDY == 0 || ADC->STATUS.bit.SYNCBUSY == 1 )
    {
      // Waiting for a complete conversion and complete synchronization
    }

    valeur += ADC->RESULT.bit.RESULT;
  }

  valeur = valeur/5;
}*/

/*
 * Arduino Zero board initialization
 *
 * Good to know:
 *   - At reset, ResetHandler did the system clock configuration. Core is running at 48MHz.
 *   - Watchdog is disabled by default, unless someone plays with NVM User page
 *   - During reset, all PORT lines are configured as inputs with input buffers, output buffers and pull disabled.
 */
void arduino_init( void )  //renamed from init to avoid clash
{
#if 1
  // Set Systick to 1ms interval, common to all Cortex-M variants
  if ( SysTick_Config( SystemCoreClock / 1000 ) )
  {
    // Capture error
    while ( 1 ) ;
  }
  NVIC_SetPriority (SysTick_IRQn,  (1 << __NVIC_PRIO_BITS) - 2);  /* set Priority for Systick Interrupt (2nd lowest) */
#endif
  // Clock PORT for Digital I/O
//  PM->APBBMASK.reg |= PM_APBBMASK_PORT ;
//
//  // Clock EIC for I/O interrupts
//  PM->APBAMASK.reg |= PM_APBAMASK_EIC ;

#if defined(__SAMD51__)
  MCLK->APBAMASK.reg |= MCLK_APBAMASK_SERCOM0 | MCLK_APBAMASK_SERCOM1 | MCLK_APBAMASK_TC0 | MCLK_APBAMASK_TC1;
  
  MCLK->APBBMASK.reg |= MCLK_APBBMASK_SERCOM2 | MCLK_APBBMASK_SERCOM3 | MCLK_APBBMASK_TCC0 | MCLK_APBBMASK_TCC1 | MCLK_APBBMASK_TC3 | MCLK_APBBMASK_TC2;
  
  MCLK->APBCMASK.reg |= MCLK_APBCMASK_TCC2 | MCLK_APBCMASK_TCC3 | MCLK_APBCMASK_TC4 | MCLK_APBCMASK_TC5;
  
  MCLK->APBDMASK.reg |= MCLK_APBDMASK_DAC | MCLK_APBDMASK_SERCOM4 | MCLK_APBDMASK_SERCOM5 | MCLK_APBDMASK_ADC0 | MCLK_APBDMASK_ADC1 | MCLK_APBDMASK_TCC4
		  | MCLK_APBDMASK_TC6 | MCLK_APBDMASK_TC7 | MCLK_APBDMASK_SERCOM6 | MCLK_APBDMASK_SERCOM7;

#else
  // Clock SERCOM for Serial
  PM->APBCMASK.reg |= PM_APBCMASK_SERCOM0 | PM_APBCMASK_SERCOM1 | PM_APBCMASK_SERCOM2 | PM_APBCMASK_SERCOM3 | PM_APBCMASK_SERCOM4 | PM_APBCMASK_SERCOM5 ;

  // Clock TC/TCC for Pulse and Analog
  PM->APBCMASK.reg |= PM_APBCMASK_TCC0 | PM_APBCMASK_TCC1 | PM_APBCMASK_TCC2 | PM_APBCMASK_TC3 | PM_APBCMASK_TC4 | PM_APBCMASK_TC5 | PM_APBCMASK_TC6 | PM_APBCMASK_TC7;

  // ATSAMR, for example, doesn't have a DAC
  #ifdef PM_APBCMASK_DAC
   // Clock ADC/DAC for Analog
   PM->APBCMASK.reg |= PM_APBCMASK_ADC | PM_APBCMASK_DAC ;
  #endif
#endif

/* 
  Commented out to leave pins in default tri-state.  This is
  aimed at avoiding power consumption in DeepSleep.
  
  // Setup all pins (digital and analog) in INPUT mode (default is nothing)
  for (uint32_t ul = 0 ; ul < NUM_DIGITAL_PINS ; ul++ )
  {
    pinMode( ul, INPUT ) ;
  }
*/

  // Initialize Analog Controller
  // Setting clock
#if defined(__SAMD51__)
  //set to 1/(1/(48000000/32) * 6) = 250000 SPS
	GCLK->PCHCTRL[ADC0_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK1_Val | (1 << GCLK_PCHCTRL_CHEN_Pos); //use clock generator 1 (48Mhz)
	GCLK->PCHCTRL[ADC1_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK1_Val | (1 << GCLK_PCHCTRL_CHEN_Pos); //use clock generator 1 (48Mhz)
	Adc *adcs[] = {ADC0, ADC1};
		for(int i=0; i<2; i++){

		adcs[i]->CTRLA.bit.PRESCALER = ADC_CTRLA_PRESCALER_DIV32_Val;
		adcs[i]->CTRLB.bit.RESSEL = ADC_CTRLB_RESSEL_10BIT_Val;

		while( adcs[i]->SYNCBUSY.reg & ADC_SYNCBUSY_CTRLB );  //wait for sync

		adcs[i]->SAMPCTRL.reg = 5;                        // sampling Time Length

		while( adcs[i]->SYNCBUSY.reg & ADC_SYNCBUSY_SAMPCTRL );  //wait for sync

		adcs[i]->INPUTCTRL.reg = ADC_INPUTCTRL_MUXNEG_GND;   // No Negative input (Internal Ground)

		while( adcs[i]->SYNCBUSY.reg & ADC_SYNCBUSY_INPUTCTRL );  //wait for sync

		// Averaging (see datasheet table in AVGCTRL register description)
		adcs[i]->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_1 |    // 1 sample only (no oversampling nor averaging)
							ADC_AVGCTRL_ADJRES(0x0ul);   // Adjusting result by 0

		while( adcs[i]->SYNCBUSY.reg & ADC_SYNCBUSY_AVGCTRL );  //wait for sync
	}

	analogReference( AR_DEFAULT ) ; // Analog Reference is AREF pin (3.3v)

	
	GCLK->PCHCTRL[DAC_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK4_Val | (1 << GCLK_PCHCTRL_CHEN_Pos); //use clock generator 4 (12mhz)
	while (GCLK->PCHCTRL[DAC_GCLK_ID].bit.CHEN == 0);
	
	while ( DAC->SYNCBUSY.bit.SWRST == 1 ); // Wait for synchronization of registers between the clock domains
	DAC->CTRLA.bit.SWRST = 1;
	while ( DAC->SYNCBUSY.bit.SWRST == 1 ); // Wait for synchronization of registers between the clock domains
	
	DAC->CTRLB.reg = DAC_CTRLB_REFSEL_VREFPU; // TODO: fix this once silicon bug is fixed
	
	//set refresh rates
	DAC->DACCTRL[0].bit.REFRESH = 2;
	DAC->DACCTRL[1].bit.REFRESH = 2;

#else
  //set to 1/(1/(48000000/32) * 6) = 250000 SPS

  while(GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);

  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID( GCM_ADC ) | // Generic Clock ADC
                      GCLK_CLKCTRL_GEN_GCLK0     | // Generic Clock Generator 0 is source
                      GCLK_CLKCTRL_CLKEN ;

  while( ADC->STATUS.bit.SYNCBUSY == 1 );          // Wait for synchronization of registers between the clock domains

  ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV32 |    // Divide Clock by 32.
                   ADC_CTRLB_RESSEL_10BIT;         // 10 bits resolution as default

  ADC->SAMPCTRL.reg = 5;                        // Sampling Time Length

  while( ADC->STATUS.bit.SYNCBUSY == 1 );          // Wait for synchronization of registers between the clock domains

  ADC->INPUTCTRL.reg = ADC_INPUTCTRL_MUXNEG_GND;   // No Negative input (Internal Ground)

  // Averaging (see datasheet table in AVGCTRL register description)
  ADC->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_1 |    // 1 sample only (no oversampling nor averaging)
                     ADC_AVGCTRL_ADJRES(0x0ul);   // Adjusting result by 0

//  analogReference( AR_DEFAULT ) ; // Analog Reference is AREF pin (3.3v)
        //replace with code that would have been called
#if 1
      while (DAC->STATUS.bit.SYNCBUSY == 1){;}// => syncADC();
      ADC->INPUTCTRL.bit.GAIN = ADC_INPUTCTRL_GAIN_DIV2_Val;
      ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTVCC1_Val; // 1/2 VDDANA =$
#endif

  // Initialize DAC
  // Setting clock
  while ( GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY );
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID( GCM_DAC ) | // Generic Clock ADC
                      GCLK_CLKCTRL_GEN_GCLK0     | // Generic Clock Generator 0 is source
                      GCLK_CLKCTRL_CLKEN ;

 // ATSAMR, for example, doesn't have a DAC
 #ifdef DAC
  while ( DAC->STATUS.bit.SYNCBUSY == 1 ); // Wait for synchronization of registers between the clock domains
  DAC->CTRLB.reg = DAC_CTRLB_REFSEL_AVCC | // Using the 3.3V reference
                   DAC_CTRLB_EOEN ;        // External Output Enable (Vout)
 #endif


#endif //SAMD51
}

#ifdef __cplusplus
}
#endif
/////////////////////////////////////////////////////////////////////////////////////////////////////

/* init -- main program, creates application processes */
void init(void);

extern void SystemInit(void);
extern void arduino_init(void);

void default_start(void)
{
    init();                    /* Call the main program. */
    while (1) pause();         /* Halt if init() returns */
}

void __start(void) __attribute((weak, alias("default_start")));

/* The next four routines can be used in C compiler output, even if
not mentioned in the source. */

/* memcpy -- copy n bytes from src to dest (non-overlapping) */
void *memcpy(void *dest, const void *src, unsigned n)
{
    unsigned char *p = dest;
    const unsigned char *q = src;
    while (n-- > 0) *p++ = *q++;
    return dest;
}

/* memmove -- copy n bytes from src to dest, allowing overlaps */
void *memmove(void *dest, const void *src, unsigned n)
{
    unsigned char *p = dest;
    const unsigned char *q = src;
    if (dest <= src)
        while (n-- > 0) *p++ = *q++;
    else {
        p += n; q += n;
        while (n-- > 0) *--p = *--q;
    }
    return dest;
}
    
/* memset -- set n bytes of dest to byte x */
void *memset(void *dest, unsigned x, unsigned n)
{
    unsigned char *p = dest;
    while (n-- > 0) *p++ = x;
    return dest;
}

/* memcmp -- compare n bytes */
int memcmp(const void *pp, const void *qq, int n)
{
    const unsigned char *p = pp, *q = qq;
    while (n-- > 0) {
        if (*p++ != *q++)
            return (p[-1] < q[-1] ? -1 : 1);
    }
    return 0;
}

/* Addresses set by the linker */
extern unsigned char __data_start[], __data_end[],
    __bss_start[], __bss_end[], __etext[], __stack[];

/* __reset -- the system starts here */

#if 1
void __reset(void)
{
//    /* Activate the crystal clock */
//    CLOCK.HFCLKSTARTED = 0;
//    CLOCK.HFCLKSTART = 1;
//    while (! CLOCK.HFCLKSTARTED) { }

    /*already running at 48MHz came from BOOTLOADER*/
    /*vector table pointer pre adjusted to 0x2000 by BOOTLOADER*/
    int data_size = __data_end - __data_start;
    int bss_size = __bss_end - __bss_start;
    memcpy(__data_start, __etext, data_size);
    memset(__bss_start, 0, bss_size);

#if defined(__FPU_USED) && defined(__SAMD51__)
        /* Enable FPU */
        SCB->CPACR |= (0xFu << 20);
        __DSB();
        __ISB();
#endif
    SystemInit(); /*clocks*/
    arduino_init();

    __start();  /*call weak default_start()*/
}
#endif

/* NVIC SETUP FUNCTIONS */

/* On Cortex-M0, only the top two bits of each interrupt priority are
implemented, but for portability priorities should be specified with
integers in the range [0..255].  On Cortex-M4, the top three bits are
implemented.*/


//njh TODO need to checkout this function
/* irq_priority -- set priority for an IRQ to a value [0..255] */
#if 0
void irq_priority(int irq, unsigned prio)
{
    if (irq < 0)
//?        SET_BYTE(SCB->SHP[(irq+12) >> 2], irq & 0x3, prio);
        SET_BYTE(M_SCB.SHPR[(irq+12) >> 2], irq & 0x3, prio);
    else
//?        SET_BYTE(NVIC->IPR[irq >> 2], irq & 0x3, prio);
        SET_BYTE(M_NVIC.ISPR[irq >> 2], irq & 0x3, prio);
}
#endif 
/* See hardware.h for macros enable_irq, disable_irq, 
clear_pending, reschedule */

#ifdef ADVANCED_STAGE_OF_DEVELOPMENT 
/* Device register arrays */
volatile _DEVICE _i2c * const I2C[1] = {
    &I2C0
};

volatile _DEVICE _timer * const TIMER[3] = {
    &TIMER0, &TIMER1, &TIMER2
};
#endif

/*  INTERRUPT VECTORS */

/* We use the linker script to define each handler name as an alias
for default_handler if it is not defined elsewhere.  Applications can
subsitute their own definitions for individual handler names like
uart_handler(). */

/* delay_loop -- timed delay */
void delay_loop(unsigned usecs)
{
//TODO revisit probably should have been opposite sense

#if defined(__SAMD51__)
//quick and dirty adjust from 16MHz to 48MHz
    unsigned t = usecs << 2;
    while (t > 0) {
        // (500/3)nsec per iteration at 48MHz
        nop(); nop(); nop();
        t--;
    }
    t = usecs << 2;
    while (t > 0) {
        // (500/3)nsec per iteration at 48MHz
        nop(); nop(); nop();
        t--;
    }
    t = usecs << 2;
    while (t > 0) {
        // (500/3)nsec per iteration at 48MHz
        nop(); nop(); nop();
        t--;
    }
#else
    delayMicroseconds(usecs);
#endif
}

/* spin -- show Seven Stars of Death */
void spin(void)
{
    intr_disable();
#if 0 
    GPIO.DIR = 0xfff0;
    while (1) {
        GPIO.OUT = 0x4000;
        delay_loop(500000);
        GPIO.OUT = 0;
        delay_loop(100000);
    }
#else
/*TODO trinket SPECIFIC should flash trinket BUILTINLED do DOTSTAR later*/
//#define GPIO_BASE 0x41004400
//#define PORTDIRSET 0x41004408
//#define PORTOUT 0x41004410
//#define PORTCLR 0x41004414
#define LED_GPIO_BIT PIN_REAL_LED
/** GPIO Register set */
//    *(unsigned int*)PORTDIRSET |= (1 << LED_GPIO_BIT);
uint32_t port = 0;
const uint32_t pin = PIN_REAL_LED;
const uint32_t pinMask = (1ul << pin);
      // enable input, to support reading back values, with pullups disabled
      PORT->Group[port].PINCFG[pin].reg = (uint8_t) (PORT_PINCFG_INEN | PORT_PINCFG_DRVSTR);
      // Set pin to output mode
      PORT->Group[port].DIRSET.reg = pinMask;



    while (1) {

//                *(unsigned int*)PORTOUT = (1 << LED_GPIO_BIT);
		PORT->Group[port].OUTSET.reg = pinMask;

        	delay_loop(500000);

//                *(unsigned int*)PORTCLR = (1 << LED_GPIO_BIT);
		PORT->Group[port].OUTCLR.reg = pinMask;
        	delay_loop(100000);

    }
#endif
}

void default_handler(void) __attribute((weak, alias("spin")));

#ifdef MICROBITSTUFF
//nrf51 specifics in here
/* The linker script makes all these handlers into weak aliases for */
/* default_handler. */

void nmi_handler(void);
void hardfault_handler(void);
void svc_handler(void);
void pendsv_handler(void);
void systick_handler(void);
void uart_handler(void);
void timer0_handler(void);
void timer1_handler(void);
void timer2_handler(void);
void power_clock_handler(void);
void radio_handler(void);
void i2c_handler(void);
void spi_handler(void);
void gpiote_handler(void);
void adc_handler(void);
void rtc0_handler(void);
void temp_handler(void);
void rng_handler(void);
void ecb_handler(void);
void ccm_aar_handler(void);
void wdt_handler(void);
void rtc1_handler(void);
void qdec_handler(void);
void lpcomp_handler(void);
void swi0_handler(void);
void swi1_handler(void);
void swi2_handler(void);
void swi3_handler(void);
void swi4_handler(void);
void swi5_handler(void);

/* This vector table is placed at address 0 in the flash by directives
in the linker script. */

void *__vectors[] __attribute((section(".vectors"))) = {
    __stack,                    /* -16 */
    __reset,
    nmi_handler,
    hardfault_handler,
    0,                          /* -12 */
    0,
    0,
    0,
    0,                          /*  -8 */
    0,
    0,
    svc_handler,
    0,                          /* -4 */
    0,
    pendsv_handler,
    systick_handler,
    
    /* external interrupts */
    power_clock_handler,        /*  0 */
    radio_handler,
    uart_handler,
    i2c_handler,
    spi_handler,                /*  4 */
    0,
    gpiote_handler,
    adc_handler,
    timer0_handler,             /*  8 */
    timer1_handler,
    timer2_handler,
    rtc0_handler,
    temp_handler,               /* 12 */
    rng_handler,
    ecb_handler,
    ccm_aar_handler,
    wdt_handler,                /* 16 */
    rtc1_handler,
    qdec_handler,
    lpcomp_handler,
    swi0_handler,               /* 20 */
    swi1_handler,
    swi2_handler,
    swi3_handler,
    swi4_handler,               /* 24 */
    swi5_handler,
    0,
    0,
    0,                          /* 28 */
    0,
    0,
    0
};
#else

//rely on cortex_handler.c
/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.
  SAMD51 support added by Adafruit - Copyright (c) 2018 Dean Miller for Adafruit Industries

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <sam.h>
#include <stddef.h> //for NULL
//#include <variant.h>
//#include <stdio.h>

/* RTOS Hooks */
//extern void svcHook(void);
//extern void pendSVHook(void);
//extern int sysTickHook(void);

extern void SysTick_DefaultHandler(void);


/* Default empty handler */
void Dummy_Handler(void)
{
#if defined DEBUG
  __BKPT(3);
#endif
  for (;;) { }
}


#if defined(__SAMD51__)

/* Cortex-M4 processor handlers */
//void Reset_Handler               ( void );
void __reset			 ( void );
void NMI_Handler                 ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void HardFault_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void MemManage_Handler           ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void BusFault_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void UsageFault_Handler          ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
//void SVC_Handler                 ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void svc_handler		 ( void );
void DebugMon_Handler            ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
//void PendSV_Handler              ( void ) __attribute__ ((weak, alias("Dummy_Handler")));
void pendsv_handler		 ( void );
//void SysTick_Handler			 ( void );
void systick_handler		 ( void );

/* Peripherals handlers */
void PM_Handler                  ( void );
void MCLK_Handler                ( void );
void OSCCTRL_0_Handler           ( void );
void OSCCTRL_1_Handler           ( void );
void OSCCTRL_2_Handler           ( void );
void OSCCTRL_3_Handler           ( void );
void OSCCTRL_4_Handler           ( void );
void OSC32KCTRL_Handler          ( void );
void SUPC_0_Handler              ( void );
void SUPC_1_Handler              ( void );
void WDT_Handler                 ( void );
void RTC_Handler                 ( void );
void EIC_0_Handler               ( void );
void EIC_1_Handler               ( void );
void EIC_2_Handler               ( void );
void EIC_3_Handler               ( void );
void EIC_4_Handler               ( void );
void EIC_5_Handler               ( void );
void EIC_6_Handler               ( void );
void EIC_7_Handler               ( void );
void EIC_8_Handler               ( void );
void EIC_9_Handler               ( void );
void EIC_10_Handler              ( void );
void EIC_11_Handler              ( void );
void EIC_12_Handler              ( void );
void EIC_13_Handler              ( void );
void EIC_14_Handler              ( void );
void EIC_15_Handler              ( void );
void FREQM_Handler               ( void );
void NVMCTRL_0_Handler           ( void );
void NVMCTRL_1_Handler           ( void );
void DMAC_0_Handler              ( void );
void DMAC_1_Handler              ( void );
void DMAC_2_Handler              ( void );
void DMAC_3_Handler              ( void );
void DMAC_4_Handler              ( void );
void EVSYS_0_Handler             ( void );
void EVSYS_1_Handler             ( void );
void EVSYS_2_Handler             ( void );
void EVSYS_3_Handler             ( void );
void EVSYS_4_Handler             ( void );
void PAC_Handler                 ( void );
void TAL_0_Handler               ( void );
void TAL_1_Handler               ( void );
void RAMECC_Handler              ( void );
void SERCOM0_0_Handler           ( void );
void SERCOM0_1_Handler           ( void );
void SERCOM0_2_Handler           ( void );
void SERCOM0_3_Handler           ( void );
void SERCOM1_0_Handler           ( void );
void SERCOM1_1_Handler           ( void );
void SERCOM1_2_Handler           ( void );
void SERCOM1_3_Handler           ( void );
void SERCOM2_0_Handler           ( void );
void SERCOM2_1_Handler           ( void );
void SERCOM2_2_Handler           ( void );
void SERCOM2_3_Handler           ( void );
void SERCOM3_0_Handler           ( void );
void SERCOM3_1_Handler           ( void );
void SERCOM3_2_Handler           ( void );
void SERCOM3_3_Handler           ( void );
void SERCOM4_0_Handler           ( void );
void SERCOM4_1_Handler           ( void );
void SERCOM4_2_Handler           ( void );
void SERCOM4_3_Handler           ( void );
void SERCOM5_0_Handler           ( void );
void SERCOM5_1_Handler           ( void );
void SERCOM5_2_Handler           ( void );
void SERCOM5_3_Handler           ( void );
void SERCOM6_0_Handler           ( void );
void SERCOM6_1_Handler           ( void );
void SERCOM6_2_Handler           ( void );
void SERCOM6_3_Handler           ( void );
void SERCOM7_0_Handler           ( void );
void SERCOM7_1_Handler           ( void );
void SERCOM7_2_Handler           ( void );
void SERCOM7_3_Handler           ( void );
void CAN0_Handler                ( void );
void CAN1_Handler                ( void );
void USB_0_Handler               ( void ); // __attribute__ ((weak));
void USB_1_Handler               ( void ); // __attribute__ ((weak));
void USB_2_Handler               ( void ); // __attribute__ ((weak));
void USB_3_Handler               ( void ); // __attribute__ ((weak));
void TCC0_0_Handler              ( void );
void TCC0_1_Handler              ( void );
void TCC0_2_Handler              ( void );
void TCC0_3_Handler              ( void );
void TCC0_4_Handler              ( void );
void TCC0_5_Handler              ( void );
void TCC0_6_Handler              ( void );
void TCC1_0_Handler              ( void );
void TCC1_1_Handler              ( void );
void TCC1_2_Handler              ( void );
void TCC1_3_Handler              ( void );
void TCC1_4_Handler              ( void );
void TCC2_0_Handler              ( void );
void TCC2_1_Handler              ( void );
void TCC2_2_Handler              ( void );
void TCC2_3_Handler              ( void );
void TCC3_0_Handler              ( void );
void TCC3_1_Handler              ( void );
void TCC3_2_Handler              ( void );
void TCC4_0_Handler              ( void );
void TCC4_1_Handler              ( void );
void TCC4_2_Handler              ( void );
void TC0_Handler                 ( void );
void TC1_Handler                 ( void );
void TC2_Handler                 ( void ); // __attribute__ ((weak)); //was used in Tone.cpp on arduino SAMD51 LOOKS LIKE THIS IS misinformation
void TC3_Handler                 ( void );
void TC4_Handler                 ( void );
void TC5_Handler                 ( void );
void TC6_Handler                 ( void );
void TC7_Handler                 ( void );
void PDEC_0_Handler              ( void );
void PDEC_1_Handler              ( void );
void PDEC_2_Handler              ( void );
void ADC0_0_Handler              ( void );
void ADC0_1_Handler              ( void );
void ADC1_0_Handler              ( void );
void ADC1_1_Handler              ( void );
void AC_Handler                  ( void );
void DAC_0_Handler               ( void );
void DAC_1_Handler               ( void );
void DAC_2_Handler               ( void );
void DAC_3_Handler               ( void );
void DAC_4_Handler               ( void );
void I2S_Handler                 ( void );
void PCC_Handler                 ( void );
void AES_Handler                 ( void );
void TRNG_Handler                ( void );
void ICM_Handler                 ( void );
void PUKCC_Handler               ( void );
void QSPI_Handler                ( void );
void SDHC0_Handler               ( void );
void SDHC1_Handler               ( void );

#if 0
/* Initialize segments */
extern uint32_t __etext;
extern uint32_t __data_start__;
extern uint32_t __data_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;
extern uint32_t __StackTop;
#endif

/* Exception Table */
//__attribute__ ((section(".isr_vector"))) const DeviceVectors exception_table =
 __attribute__ ((section(".vectors"))) void *__vectors[] =
{
	/* Configure Initial Stack Pointer, using linker-generated symbols */
	__stack, //(void*) (&__StackTop),
	/* Cortex-M handlers */
	__reset, //(void*) Reset_Handler,
	(void*) NMI_Handler,			    /*-14*/
	(void*) HardFault_Handler,		    /*-13*/
	(void*) MemManage_Handler,		    /*-12*/
	(void*) BusFault_Handler,		    /*-11*/
	(void*) UsageFault_Handler,		    /*-10*/
	(void*) (0UL), /* Reserved */
	(void*) (0UL), /* Reserved */
	(void*) (0UL), /* Reserved */
	(void*) (0UL), /* Reserved */
	svc_handler, //(void*) SVC_Handler,	    /*-5*/
	(void*) DebugMon_Handler,		    /*-4*/
	(void*) (0UL), /* Reserved */
	pendsv_handler, //(void*) PendSV_Handler,   /*-2*/
	systick_handler, //(void*) SysTick_Handler, /*-1*/

	/* Peripheral handlers */
	  (void*) PM_Handler,                    /*  0 Power Manager */
	  (void*) MCLK_Handler,                  /*  1 Main Clock */
	  (void*) OSCCTRL_0_Handler,             /*  2 Oscillators Control IRQ 0 */
	  (void*) OSCCTRL_1_Handler,             /*  3 Oscillators Control IRQ 1 */
	  (void*) OSCCTRL_2_Handler,             /*  4 Oscillators Control IRQ 2 */
	  (void*) OSCCTRL_3_Handler,             /*  5 Oscillators Control IRQ 3 */
	  (void*) OSCCTRL_4_Handler,             /*  6 Oscillators Control IRQ 4 */
	  (void*) OSC32KCTRL_Handler,            /*  7 32kHz Oscillators Control */
	  (void*) SUPC_0_Handler,                /*  8 Supply Controller IRQ 0 */
	  (void*) SUPC_1_Handler,                /*  9 Supply Controller IRQ 1 */
	  (void*) WDT_Handler,                   /* 10 Watchdog Timer */
	  (void*) RTC_Handler,                   /* 11 Real-Time Counter */
	  (void*) EIC_0_Handler,                 /* 12 External Interrupt Controller IRQ 0 */
	  (void*) EIC_1_Handler,                 /* 13 External Interrupt Controller IRQ 1 */
	  (void*) EIC_2_Handler,                 /* 14 External Interrupt Controller IRQ 2 */
	  (void*) EIC_3_Handler,                 /* 15 External Interrupt Controller IRQ 3 */
	  (void*) EIC_4_Handler,                 /* 16 External Interrupt Controller IRQ 4 */
	  (void*) EIC_5_Handler,                 /* 17 External Interrupt Controller IRQ 5 */
	  (void*) EIC_6_Handler,                 /* 18 External Interrupt Controller IRQ 6 */
	  (void*) EIC_7_Handler,                 /* 19 External Interrupt Controller IRQ 7 */
	  (void*) EIC_8_Handler,                 /* 20 External Interrupt Controller IRQ 8 */
	  (void*) EIC_9_Handler,                 /* 21 External Interrupt Controller IRQ 9 */
	  (void*) EIC_10_Handler,                /* 22 External Interrupt Controller IRQ 10 */
	  (void*) EIC_11_Handler,                /* 23 External Interrupt Controller IRQ 11 */
	  (void*) EIC_12_Handler,                /* 24 External Interrupt Controller IRQ 12 */
	  (void*) EIC_13_Handler,                /* 25 External Interrupt Controller IRQ 13 */
	  (void*) EIC_14_Handler,                /* 26 External Interrupt Controller IRQ 14 */
	  (void*) EIC_15_Handler,                /* 27 External Interrupt Controller IRQ 15 */
	  (void*) FREQM_Handler,                 /* 28 Frequency Meter */
	  (void*) NVMCTRL_0_Handler,             /* 29 Non-Volatile Memory Controller IRQ 0 */
	  (void*) NVMCTRL_1_Handler,             /* 30 Non-Volatile Memory Controller IRQ 1 */
	  (void*) DMAC_0_Handler,                /* 31 Direct Memory Access Controller IRQ 0 */
	  (void*) DMAC_1_Handler,                /* 32 Direct Memory Access Controller IRQ 1 */
	  (void*) DMAC_2_Handler,                /* 33 Direct Memory Access Controller IRQ 2 */
	  (void*) DMAC_3_Handler,                /* 34 Direct Memory Access Controller IRQ 3 */
	  (void*) DMAC_4_Handler,                /* 35 Direct Memory Access Controller IRQ 4 */
	  (void*) EVSYS_0_Handler,               /* 36 Event System Interface IRQ 0 */
	  (void*) EVSYS_1_Handler,               /* 37 Event System Interface IRQ 1 */
	  (void*) EVSYS_2_Handler,               /* 38 Event System Interface IRQ 2 */
	  (void*) EVSYS_3_Handler,               /* 39 Event System Interface IRQ 3 */
	  (void*) EVSYS_4_Handler,               /* 40 Event System Interface IRQ 4 */
	  (void*) PAC_Handler,                   /* 41 Peripheral Access Controller */
	  (void*) TAL_0_Handler,                 /* 42 Trigger Allocator IRQ 0 */
	  (void*) TAL_1_Handler,                 /* 43 Trigger Allocator IRQ 1 */
	  (void*) (0UL),
	  (void*) RAMECC_Handler,                /* 45 RAM ECC */
	  (void*) SERCOM0_0_Handler,             /* 46 Serial Communication Interface 0 IRQ 0 */
	  (void*) SERCOM0_1_Handler,             /* 47 Serial Communication Interface 0 IRQ 1 */
	  (void*) SERCOM0_2_Handler,             /* 48 Serial Communication Interface 0 IRQ 2 */
	  (void*) SERCOM0_3_Handler,             /* 49 Serial Communication Interface 0 IRQ 3 */
	  (void*) SERCOM1_0_Handler,             /* 50 Serial Communication Interface 1 IRQ 0 */
	  (void*) SERCOM1_1_Handler,             /* 51 Serial Communication Interface 1 IRQ 1 */
	  (void*) SERCOM1_2_Handler,             /* 52 Serial Communication Interface 1 IRQ 2 */
	  (void*) SERCOM1_3_Handler,             /* 53 Serial Communication Interface 1 IRQ 3 */
	  (void*) SERCOM2_0_Handler,             /* 54 Serial Communication Interface 2 IRQ 0 */
	  (void*) SERCOM2_1_Handler,             /* 55 Serial Communication Interface 2 IRQ 1 */
	  (void*) SERCOM2_2_Handler,             /* 56 Serial Communication Interface 2 IRQ 2 */
	  (void*) SERCOM2_3_Handler,             /* 57 Serial Communication Interface 2 IRQ 3 */
	  (void*) SERCOM3_0_Handler,             /* 58 Serial Communication Interface 3 IRQ 0 */
	  (void*) SERCOM3_1_Handler,             /* 59 Serial Communication Interface 3 IRQ 1 */
	  (void*) SERCOM3_2_Handler,             /* 60 Serial Communication Interface 3 IRQ 2 */
	  (void*) SERCOM3_3_Handler,             /* 61 Serial Communication Interface 3 IRQ 3 */
	  (void*) SERCOM4_0_Handler,             /* 62 Serial Communication Interface 4 IRQ 0 */
	  (void*) SERCOM4_1_Handler,             /* 63 Serial Communication Interface 4 IRQ 1 */
	  (void*) SERCOM4_2_Handler,             /* 64 Serial Communication Interface 4 IRQ 2 */
	  (void*) SERCOM4_3_Handler,             /* 65 Serial Communication Interface 4 IRQ 3 */
	  (void*) SERCOM5_0_Handler,             /* 66 Serial Communication Interface 5 IRQ 0 */
	  (void*) SERCOM5_1_Handler,             /* 67 Serial Communication Interface 5 IRQ 1 */
	  (void*) SERCOM5_2_Handler,             /* 68 Serial Communication Interface 5 IRQ 2 */
	  (void*) SERCOM5_3_Handler,             /* 69 Serial Communication Interface 5 IRQ 3 */
	  (void*) SERCOM6_0_Handler,             /* 70 Serial Communication Interface 6 IRQ 0 */
	  (void*) SERCOM6_1_Handler,             /* 71 Serial Communication Interface 6 IRQ 1 */
	  (void*) SERCOM6_2_Handler,             /* 72 Serial Communication Interface 6 IRQ 2 */
	  (void*) SERCOM6_3_Handler,             /* 73 Serial Communication Interface 6 IRQ 3 */
	  (void*) SERCOM7_0_Handler,             /* 74 Serial Communication Interface 7 IRQ 0 */
	  (void*) SERCOM7_1_Handler,             /* 75 Serial Communication Interface 7 IRQ 1 */
	  (void*) SERCOM7_2_Handler,             /* 76 Serial Communication Interface 7 IRQ 2 */
	  (void*) SERCOM7_3_Handler,             /* 77 Serial Communication Interface 7 IRQ 3 */
	  (void*) CAN0_Handler,                  /* 78 Control Area Network 0 (SAM E5x) */
	  (void*) CAN1_Handler,                  /* 79 Control Area Network 0 (SAM E5x) */
	  (void*) USB_0_Handler,                 /* 80 Universal Serial Bus IRQ 0 */
	  (void*) USB_1_Handler,                 /* 81 Universal Serial Bus IRQ 1 */
	  (void*) USB_2_Handler,                 /* 82 Universal Serial Bus IRQ 2 */
	  (void*) USB_3_Handler,                 /* 83 Universal Serial Bus IRQ 3 */
	  (void*) (0UL),
	  (void*) TCC0_0_Handler,                /* 85 Timer Counter Control 0 IRQ 0 */
	  (void*) TCC0_1_Handler,                /* 86 Timer Counter Control 0 IRQ 1 */
	  (void*) TCC0_2_Handler,                /* 87 Timer Counter Control 0 IRQ 2 */
	  (void*) TCC0_3_Handler,                /* 88 Timer Counter Control 0 IRQ 3 */
	  (void*) TCC0_4_Handler,                /* 89 Timer Counter Control 0 IRQ 4 */
	  (void*) TCC0_5_Handler,                /* 90 Timer Counter Control 0 IRQ 5 */
	  (void*) TCC0_6_Handler,                /* 91 Timer Counter Control 0 IRQ 6 */
	  (void*) TCC1_0_Handler,                /* 92 Timer Counter Control 1 IRQ 0 */
	  (void*) TCC1_1_Handler,                /* 93 Timer Counter Control 1 IRQ 1 */
	  (void*) TCC1_2_Handler,                /* 94 Timer Counter Control 1 IRQ 2 */
	  (void*) TCC1_3_Handler,                /* 95 Timer Counter Control 1 IRQ 3 */
	  (void*) TCC1_4_Handler,                /* 96 Timer Counter Control 1 IRQ 4 */
	  (void*) TCC2_0_Handler,                /* 97 Timer Counter Control 2 IRQ 0 */
	  (void*) TCC2_1_Handler,                /* 98 Timer Counter Control 2 IRQ 1 */
	  (void*) TCC2_2_Handler,                /* 99 Timer Counter Control 2 IRQ 2 */
	  (void*) TCC2_3_Handler,                /* 100 Timer Counter Control 2 IRQ 3 */
	  (void*) TCC3_0_Handler,                /* 101 Timer Counter Control 3 IRQ 0 */
	  (void*) TCC3_1_Handler,                /* 102 Timer Counter Control 3 IRQ 1 */
	  (void*) TCC3_2_Handler,                /* 103 Timer Counter Control 3 IRQ 2 */
	  (void*) TCC4_0_Handler,                /* 104 Timer Counter Control 4 IRQ 0 */
	  (void*) TCC4_1_Handler,                /* 105 Timer Counter Control 4 IRQ 1 */
	  (void*) TCC4_2_Handler,                /* 106 Timer Counter Control 4 IRQ 2 */
	  (void*) TC0_Handler,                   /* 107 Basic Timer Counter 0 */
	  (void*) TC1_Handler,                   /* 108 Basic Timer Counter 1 */
	  (void*) TC2_Handler,                   /* 109 Basic Timer Counter 2 */
	  (void*) TC3_Handler,                   /* 110 Basic Timer Counter 3 */
	  (void*) TC4_Handler,                   /* 111 Basic Timer Counter 4 */
	  (void*) TC5_Handler,                   /* 112 Basic Timer Counter 5 */
	  (void*) TC6_Handler,                   /* 113 Basic Timer Counter 6 */
	  (void*) TC7_Handler,                   /* 114 Basic Timer Counter 7 */
	  (void*) PDEC_0_Handler,                /* 115 Quadrature Decodeur IRQ 0 */
	  (void*) PDEC_1_Handler,                /* 116 Quadrature Decodeur IRQ 1 */
	  (void*) PDEC_2_Handler,                /* 117 Quadrature Decodeur IRQ 2 */
	  (void*) ADC0_0_Handler,                /* 118 Analog Digital Converter 0 IRQ 0 */
	  (void*) ADC0_1_Handler,                /* 119 Analog Digital Converter 0 IRQ 1 */
	  (void*) ADC1_0_Handler,                /* 120 Analog Digital Converter 1 IRQ 0 */
	  (void*) ADC1_1_Handler,                /* 121 Analog Digital Converter 1 IRQ 1 */
	  (void*) AC_Handler,                    /* 122 Analog Comparators */
	  (void*) DAC_0_Handler,                 /* 123 Digital-to-Analog Converter IRQ 0 */
	  (void*) DAC_1_Handler,                 /* 124 Digital-to-Analog Converter IRQ 1 */
	  (void*) DAC_2_Handler,                 /* 125 Digital-to-Analog Converter IRQ 2 */
	  (void*) DAC_3_Handler,                 /* 126 Digital-to-Analog Converter IRQ 3 */
	  (void*) DAC_4_Handler,                 /* 127 Digital-to-Analog Converter IRQ 4 */
	  (void*) I2S_Handler,                   /* 128 Inter-IC Sound Interface */
	  (void*) PCC_Handler,                   /* 129 Parallel Capture Controller */
	  (void*) AES_Handler,                   /* 130 Advanced Encryption Standard */
	  (void*) TRNG_Handler,                  /* 131 True Random Generator */
	  (void*) ICM_Handler,                   /* 132 Integrity Check Monitor */
	  (void*) PUKCC_Handler,                 /* 133 PUblic-Key Cryptography Controller */
	  (void*) QSPI_Handler,                  /* 134 Quad SPI interface */
	  (void*) SDHC0_Handler,                 /* 135 SD/MMC Host Controller 0 */
	  (void*) SDHC1_Handler,                 /* 136 SD/MMC Host Controller 1 */
};

#else


/* Cortex-M0+ core handlers */
void hardfault_handler(void);
void __reset          (void);
void nmi_handler      (void);
void svc_handler      (void);
void pendsv_handler   (void);
void systick_handler  (void);

/* Peripherals handlers */
void PM_Handler       (void);
void SYSCTRL_Handler  (void);
void WDT_Handler      (void);
void RTC_Handler      (void);
void EIC_Handler      (void);
void NVMCTRL_Handler  (void);
void DMAC_Handler     (void);
void USB_Handler      (void); /*special case override default_handler*/
void EVSYS_Handler    (void);
void SERCOM0_Handler  (void); /*UART*/
void SERCOM1_Handler  (void);
void SERCOM2_Handler  (void);
void SERCOM3_Handler  (void);
void SERCOM4_Handler  (void);
void SERCOM5_Handler  (void);
void TCC0_Handler     (void);
void TCC1_Handler     (void);
void TCC2_Handler     (void);
void TC3_Handler      (void);
void TC4_Handler      (void);
void TC5_Handler      (void); /* was Used in Tone.cpp on arduino*/
void TC6_Handler      (void);
void TC7_Handler      (void);
void ADC_Handler      (void);
void AC_Handler       (void);
void DAC_Handler      (void);
void PTC_Handler      (void);
void I2S_Handler      (void);

#if 0
//using modified .ld from baremetal-v1/microbian
//adjust for larger MEMORY of samd21
/* Initialize segments */
extern uint32_t __etext;
extern uint32_t __data_start__;
extern uint32_t __data_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;
extern uint32_t __StackTop;
#endif


#if 1
extern unsigned char __stack[]; 

/* Exception Table */
//__attribute__ ((section(".isr_vector"))) const DeviceVectors exception_table =
__attribute__ ((section(".vectors"))) void *__vectors[] =
{
  /* Configure Initial Stack Pointer, using linker-generated symbols */
  __stack, //(void*) (&__StackTop),
  __reset, //(void*) Reset_Handler,
  (void*) nmi_handler,                       /*-14*/
  (void*) hardfault_handler,                 /*-13*/
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  svc_handler, //(void*) SVC_Handler,         /*-5*/
  (void*) (0UL), /* Reserved */
  (void*) (0UL), /* Reserved */
  pendsv_handler, //(void*) PendSV_Handler,   /*-2*/
  systick_handler, //(void*) SysTick_Handler, /*-1*/

  /* Configurable interrupts */
  (void*) PM_Handler,             /*  0 Power Manager */
  (void*) SYSCTRL_Handler,        /*  1 System Control */
  (void*) WDT_Handler,            /*  2 Watchdog Timer */
  (void*) RTC_Handler,            /*  3 Real-Time Counter */
  (void*) EIC_Handler,            /*  4 External Interrupt Controller */
  (void*) NVMCTRL_Handler,        /*  5 Non-Volatile Memory Controller */
  (void*) DMAC_Handler,           /*  6 Direct Memory Access Controller */
  (void*) USB_Handler,            /*  7 Universal Serial Bus */
  (void*) EVSYS_Handler,          /*  8 Event System Interface */
  (void*) SERCOM0_Handler,        /*  9 Serial Communication Interface 0 */
  (void*) SERCOM1_Handler,        /* 10 Serial Communication Interface 1 */
  (void*) SERCOM2_Handler,        /* 11 Serial Communication Interface 2 */
  (void*) SERCOM3_Handler,        /* 12 Serial Communication Interface 3 */
  (void*) SERCOM4_Handler,        /* 13 Serial Communication Interface 4 */
  (void*) SERCOM5_Handler,        /* 14 Serial Communication Interface 5 */
  (void*) TCC0_Handler,           /* 15 Timer Counter Control 0 */
  (void*) TCC1_Handler,           /* 16 Timer Counter Control 1 */
  (void*) TCC2_Handler,           /* 17 Timer Counter Control 2 */
  (void*) TC3_Handler,            /* 18 Basic Timer Counter 0 */
  (void*) TC4_Handler,            /* 19 Basic Timer Counter 1 */
  (void*) TC5_Handler,            /* 20 Basic Timer Counter 2 */
  (void*) TC6_Handler,            /* 21 Basic Timer Counter 3 */
  (void*) TC7_Handler,            /* 22 Basic Timer Counter 4 */
  (void*) ADC_Handler,            /* 23 Analog Digital Converter */
  (void*) AC_Handler,             /* 24 Analog Comparators */
  (void*) DAC_Handler,            /* 25 Digital Analog Converter */
  (void*) PTC_Handler,            /* 26 Peripheral Touch Controller */
  (void*) I2S_Handler,            /* 27 Inter-IC Sound Interface */
  (void*) (0UL),                  /* Reserved */
};
#endif
#endif //MICROBITSTUFF

extern volatile uint32_t _ulTickCount;
void systick_handler(void)
{
_ulTickCount++;
//  tickReset();  //if CDC USB reset is implemented
}

static void (*usb_isr)(void) = NULL;

#if defined(__SAMD51__)
void USB_0_Handler(void)
{
	if (usb_isr)
	usb_isr();
}
void USB_1_Handler(void)
{
	if (usb_isr)
	usb_isr();
}
void USB_2_Handler(void)
{
	if (usb_isr)
	usb_isr();
}
void USB_3_Handler(void)
{
	if (usb_isr)
	usb_isr();
}
#else
void USB_Handler(void)
{
  if (usb_isr)
    usb_isr();
}
#endif

void USB_SetHandler(void (*new_usb_isr)(void))
{
  usb_isr = new_usb_isr;
}

#endif
