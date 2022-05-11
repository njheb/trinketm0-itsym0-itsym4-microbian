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

/*this file is based on Tone.cpp*/

#include "tick_timer.h"
//#include "variant.h"
#include "variant-cutdown.h"
#include "sam.h"

//uint32_t toneMaxFrequency = F_CPU / 2;
uint32_t toneMaxFrequency = F_CPU; //fix so we can ask for the frequency we want

//volatile bool firstTimeRunning = false;

#if defined(__SAMD51__)
//  #define TICK_TIMER_TC         TC0
//  #define TICK_TIMER_TC_IRQn    TC0_IRQn
  #define TICK_TIMER_TC_GCLK_ID	TC0_GCLK_ID
  #define TICK_TIMER_Handler    TC0_Handler

  #define WAIT_TC16_REGS_SYNC(x) while(x->COUNT16.SYNCBUSY.bit.ENABLE);

#else
//  #define TICK_TIMER_TC         TC5
//  #define TICK_TIMER_TC_IRQn    TC5_IRQn
  #define TICK_TIMER_Handler    TC5_Handler

  #define WAIT_TC16_REGS_SYNC(x) while(x->COUNT16.STATUS.bit.SYNCBUSY);

  #define GCM_TC4_TC5               (0x1CU)
#endif

#define TICK_TIMER_TC_TOP     0xFFFF
#define TICK_TIMER_TC_CHANNEL 0

static inline void resetTC (Tc* TCx)
{
  // Disable TCx
  TCx->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  WAIT_TC16_REGS_SYNC(TCx)

  // Reset TCx
  TCx->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
  WAIT_TC16_REGS_SYNC(TCx)
  while (TCx->COUNT16.CTRLA.bit.SWRST);
}

void toneAccurateClock (uint32_t accurateSystemCoreClockFrequency)
{
  toneMaxFrequency = accurateSystemCoreClockFrequency / 2;
}

//void tone (uint32_t frequency)
void tick_timer_setup(uint32_t frequency)
{

  // Avoid divide by zero error by calling 'noTone' instead
  if (frequency == 0)
  {
    panic("tone tick DC");
  }

  // Configure interrupt request
  NVIC_DisableIRQ(TICK_TIMER_TC_IRQn);
  NVIC_ClearPendingIRQ(TICK_TIMER_TC_IRQn);
  
//  if(!firstTimeRunning)
  {
//    firstTimeRunning = true;
    
//    NVIC_SetPriority(TICK_TIMER_TC_IRQn, 5);//what would 5 mean as originally present
    NVIC_SetPriority(TICK_TIMER_TC_IRQn, 1);  //0=0x00 1=0x40 2=0x80  3=0xc0
//tried 5, 3, 0 (0 works may not be correct value), 1
//with 0 seeing odd scheduling on itsym0 don't  know if it was the case on itsym4 
// 1 fixes odd scheduling, but don't know if it should be 1 or 2

#if defined(__SAMD51__)
    GCLK->PCHCTRL[TICK_TIMER_TC_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK0_Val | (1 << GCLK_PCHCTRL_CHEN_Pos);
#else
    // Enable GCLK for TC4 and TC5 (timer counter input clock)
    GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TC4_TC5));
    while (GCLK->STATUS.bit.SYNCBUSY);
#endif
  }


  //
  // Calculate best prescaler divider and comparator value for a 16 bit TC peripheral
  //

  uint32_t prescalerConfigBits;
  uint32_t ccValue;

  ccValue = toneMaxFrequency / frequency - 1;
  prescalerConfigBits = TC_CTRLA_PRESCALER_DIV1;

  uint8_t i = 0;

  while(ccValue > TICK_TIMER_TC_TOP)
  {
    ccValue = toneMaxFrequency / frequency / (2<<i) - 1;
    i++;
    if(i == 4 || i == 6 || i == 8) //DIV32 DIV128 and DIV512 are not available
     i++;
  }

  switch(i-1)
  {
    case 0: prescalerConfigBits = TC_CTRLA_PRESCALER_DIV2; break;

    case 1: prescalerConfigBits = TC_CTRLA_PRESCALER_DIV4; break;

    case 2: prescalerConfigBits = TC_CTRLA_PRESCALER_DIV8; break;

    case 3: prescalerConfigBits = TC_CTRLA_PRESCALER_DIV16; break;

    case 5: prescalerConfigBits = TC_CTRLA_PRESCALER_DIV64; break;

    case 7: prescalerConfigBits = TC_CTRLA_PRESCALER_DIV256; break;

    case 9: prescalerConfigBits = TC_CTRLA_PRESCALER_DIV1024; break;

    default: break;
  }

  resetTC(TICK_TIMER_TC);

  uint16_t tmpReg = 0;
  tmpReg |= TC_CTRLA_MODE_COUNT16;  // Set Timer counter Mode to 16 bits

#if defined(__SAMD51__)
	TICK_TIMER_TC->COUNT16.WAVE.reg = TC_WAVE_WAVEGEN_MFRQ;  // Set TICK_TIMER_TC mode as match frequency
#else
  tmpReg |= TC_CTRLA_WAVEGEN_MFRQ;  // Set TICK_TIMER_TC mode as match frequency
#endif
  tmpReg |= prescalerConfigBits;
  TICK_TIMER_TC->COUNT16.CTRLA.reg |= tmpReg;
  WAIT_TC16_REGS_SYNC(TICK_TIMER_TC)

  TICK_TIMER_TC->COUNT16.CC[TICK_TIMER_TC_CHANNEL].reg = (uint16_t) ccValue;
  WAIT_TC16_REGS_SYNC(TICK_TIMER_TC)

  // Enable the TICK_TIMER_TC interrupt request
  TICK_TIMER_TC->COUNT16.INTENSET.bit.MC0 = 1;

  // Enable TICK_TIMER_TC
  TICK_TIMER_TC->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
  WAIT_TC16_REGS_SYNC(TICK_TIMER_TC)

//  NVIC_EnableIRQ(TICK_TIMER_TC_IRQn);
}



