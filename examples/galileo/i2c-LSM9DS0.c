/*
 * Copyright (C) 2015-2016, Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <string.h>

#include "contiki.h"
#include "sys/ctimer.h"

#include "i2c.h"

#define LSM9DS0_I2C_ADDR 0x6A
#define WHO_AM_I_ADDR    0x0F
#define WHO_AM_I_ANSWER  0xD4

static uint8_t tx_data = WHO_AM_I_ADDR;
static uint8_t rx_data = 0;
static struct ctimer timer;

PROCESS(i2c_lsm9ds0_process, "I2C LSM9DS0 Who Am I Process");
AUTOSTART_PROCESSES(&i2c_lsm9ds0_process);
/*---------------------------------------------------------------------------*/
static void
rx(void)
{
  if (rx_data == WHO_AM_I_ANSWER)
    printf("Who am I register value match!\n");
  else
    printf("Who am I register value DOESN'T match! %u\n", rx_data);
}
/*---------------------------------------------------------------------------*/
static void
tx(void)
{
  rx_data = 0;

  quarkX1000_i2c_read(&rx_data, sizeof(rx_data), LSM9DS0_I2C_ADDR);
}
/*---------------------------------------------------------------------------*/
static void
err(void)
{
  printf("Something went wrong. err() callback has been called.\n");
}
/*---------------------------------------------------------------------------*/
static void
timeout(void *data)
{
  quarkX1000_i2c_write(&tx_data, sizeof(tx_data), LSM9DS0_I2C_ADDR);

  ctimer_reset(&timer);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(i2c_lsm9ds0_process, ev, data)
{
  PROCESS_BEGIN();

  quarkX1000_i2c_set_callbacks(rx, tx, err);

  ctimer_set(&timer, CLOCK_SECOND * 5, timeout, NULL);

  printf("I2C LSM9DS0 uart_demo is running\n");

  PROCESS_YIELD();

  PROCESS_END();
}
