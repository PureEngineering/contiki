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
#include "contiki.h"
#include "stm32l1xx_conf.h"
#include "rtc-arch.h"
#include "lpm-arch.h"
/*---------------------------------------------------------------------------*/
#define DEBUG 0
#if DEBUG
  #define PRINTF(...)	printf(__VA_ARGS__)
#else
  #define PRINTF(...)
#endif
/*---------------------------------------------------------------------------*/
/* The counter assumes the timer is sourced with the LSE and uses Div16 */
#define RTC_WKUPCOUNTER			((F_LSE/16) / CLOCK_SECOND)
/*---------------------------------------------------------------------------*/
static volatile unsigned long seconds;
static volatile clock_time_t ticks;
/*---------------------------------------------------------------------------*/
void RTC_WKUP_IRQHandler(void)
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  /* Check on the WakeUp flag */
  if(RTC_GetITStatus(RTC_IT_WUT) != RESET) 
  {
    ticks++;
    if((ticks % CLOCK_SECOND) == 0){
      seconds++;
      energest_flush();
      PRINTF("second %i (%i ticks)\n", seconds, ticks);
    }

    /* If an etimer expired, continue its process */
    if(etimer_pending()){
      lpm_exit_stopmode();
      etimer_request_poll();
    }

    /* Clear RTC WakeUp flags */
    RTC_ClearITPendingBit(RTC_IT_WUT);
  }

  /* Clear the EXTI line 20 */
  EXTI_ClearITPendingBit(EXTI_Line20);

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/
void clock_init(void)
{
  seconds = 0;
  ticks = 0;

  /* Initialize the RTC clock */
  init_rtc();

  /* Initialize the RTC WakeUp interrupt */
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* EXTI configuration */
  EXTI_ClearITPendingBit(EXTI_Line20);
  EXTI_InitStructure.EXTI_Line = EXTI_Line20;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable the RTC Wakeup Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
 
  /* RTC Wakeup Interrupt Generation: Clock Source: RTCDiv_16, Wakeup Time Base: 7.8ms at 128Hz */
  RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
  RTC_SetWakeUpCounter(RTC_WKUPCOUNTER-1);

  /* Enable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, ENABLE);

  /* Enable Wakeup Counter */
  RTC_WakeUpCmd(ENABLE);
}
/*---------------------------------------------------------------------------*/
unsigned long clock_seconds(void)
{
  return seconds;
}
/*---------------------------------------------------------------------------*/
void clock_set_seconds(unsigned long sec)
{
  seconds = sec;
}
/*---------------------------------------------------------------------------*/
clock_time_t clock_time(void)
{
  return ticks;
}
/*---------------------------------------------------------------------------*/
/* Busy-wait the CPU for a duration depending on CPU speed */
void clock_delay(unsigned int i)
{
  for(; i > 0; i--) {
    unsigned int j;
    for(j = 50; j > 0; j--) {
      __NOP();
    }
  }
}
/*---------------------------------------------------------------------------*/
/* Wait for a multiple of clock ticks (7.8ms per tick at 128Hz) */
void clock_wait(clock_time_t i)
{
  clock_time_t start;
  start = clock_time();
  while(clock_time() - start < i);
}
/*---------------------------------------------------------------------------*/
