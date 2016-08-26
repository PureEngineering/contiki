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
#include "dev/leds.h"
#include "lora-contiki-interface.h"
/*---------------------------------------------------------------------------*/
void leds_arch_init(void)
{
  /* Initialize LED structures */
  GpioInit(&Led1, LED_1, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
  GpioInit(&Led2, LED_2, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
  GpioInit(&Led3, LED_3, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0);
}
/*---------------------------------------------------------------------------*/
unsigned char leds_arch_get(void)
{
  /* Create LED bitmask using bitwise OR'ing the seperate values */
  return (GpioRead(&Led1) ? LEDS_RED : 0)
	| (GpioRead(&Led2) ? LEDS_GREEN : 0)
	| (GpioRead(&Led3) ? LEDS_YELLOW : 0);
}
/*---------------------------------------------------------------------------*/
void leds_arch_set(unsigned char leds)
{
  /* Test the LED bitmask and set the LEDs accordingly */
  GpioWrite(&Led1, (leds & LEDS_RED) ? 0 : 1);
  GpioWrite(&Led2, (leds & LEDS_GREEN) ? 0 : 1);
  GpioWrite(&Led3, (leds & LEDS_YELLOW) ? 0 : 1);
}
/*---------------------------------------------------------------------------*/
