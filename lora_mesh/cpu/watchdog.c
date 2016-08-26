/*
 * Copyright (c) 2012, STMicroelectronics.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */
/*---------------------------------------------------------------------------*/
#include "dev/watchdog.h"
#include "stm32l1xx_conf.h"
#include "contiki-conf.h"
/*---------------------------------------------------------------------------*/
static uint8_t counterValue;
/*---------------------------------------------------------------------------*/
void watchdog_init(void)
{
#if WATCHDOG_USE_IWDG
  /* Get the LSI frequency: 37kHz according to stm32l1x datasheet,
     but can be measured with a high speed oscillator for greater precision */
  uint32_t LsiFreq = F_LSI;

  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

  /* IWDG counter clock: LSI/32 */
  IWDG_SetPrescaler(IWDG_Prescaler_32);

  /* Set counter reload value to obtain 250ms IWDG TimeOut.
     (the timeout may varies due to LSI frequency dispersion)
     Counter Reload Value = IWDG counter clock period/250ms
              = (LSI/32) / 250ms
              = (LsiFreq/32) / 0.25s
              = LsiFreq / (32 * 4)
              = LsiFreq / 128
  */
  IWDG_SetReload(LsiFreq/(32 * (1000/WATCHDOG_IWDG_TIMEOUT)));
#else
  /* Enable WWDG clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

  /* WWDG clock counter = (PCLK1 (32MHz)/4096)/8 = 977 Hz (~1024 us) */
  WWDG_SetPrescaler(WWDG_Prescaler_8);

  /* Set Window value to 127; WWDG counter should be refreshed only when the counter
     is below 127 (and greater than 64) otherwise a reset will be generated */
  WWDG_SetWindowValue(127);

  /* Set counter value to 127; WWDG timeout = ~1024 us * 64 = 65.53 ms
     In this case the refresh window is:
     ~1024us * (127-127) = 0 ms < refresh window < ~1024us * 64 = 65.53ms */
  counterValue = 127;
#endif
}
/*---------------------------------------------------------------------------*/
void watchdog_start(void)
{
#if ENABLE_WATCHDOG
  /* We setup the watchdog to reset the device after a specific time,
     unless watchdog_periodic() is called */
  #if WATCHDOG_USE_IWDG
    IWDG_ReloadCounter();
    IWDG_Enable();
  #else
    WWDG_Enable(counterValue);
  #endif
#endif
}
/*---------------------------------------------------------------------------*/
void watchdog_periodic(void)
{
  /* This function is called periodically to restart the watchdog timer */
#if WATCHDOG_USE_IWDG
  IWDG_ReloadCounter();
#else
  WWDG_SetCounter(counterValue);
#endif
}
/*---------------------------------------------------------------------------*/
void watchdog_stop(void)
{
  /* Impossible to stop watchdogs once started */
}
/*---------------------------------------------------------------------------*/
void watchdog_reboot(void)
{
  watchdog_stop();
  watchdog_init();
  watchdog_start();
}
/*---------------------------------------------------------------------------*/
