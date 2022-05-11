/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.

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

#pragma once

//#ifdef __cplusplus

//#include "Arduino.h"
#include <stdint.h>
void tone(uint32_t frequency);

//void tone(uint32_t _pin, uint32_t frequency, uint32_t duration = 0);
//void noTone(uint32_t _pin);

#if defined(__SAMD51__)
  #define TICK_TIMER_TC         TC0
  #define TICK_TIMER_TC_IRQn    TC0_IRQn
//  #define TICK_TIMER_TC_GCLK_ID       TC0_GCLK_ID
  #define TICK_TIMER_Handler    TC0_Handler

//  #define WAIT_TC16_REGS_SYNC(x) while(x->COUNT16.SYNCBUSY.bit.ENABLE);

#else
  #define TICK_TIMER_TC         TC5
  #define TICK_TIMER_TC_IRQn    TC5_IRQn
  #define TICK_TIMER_Handler    TC5_Handler

//  #define WAIT_TC16_REGS_SYNC(x) while(x->COUNT16.STATUS.bit.SYNCBUSY);

//  #define GCM_TC4_TC5               (0x1CU)
#endif

//#endif
