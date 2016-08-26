/*
 * Copyright (c) 2005, Swedish Institute of Computer Science
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
#ifndef __PLATFORM_CONF_H__
#define __PLATFORM_CONF_H__
/*---------------------------------------------------------------------------*/
#include <inttypes.h>
#include <string.h>
#include "lora-radio-arch-config.h"
/*---------------------------------------------------------------------------*/

#define PLATFORM_HAS_LEDS		1
#define PLATFORM_HAS_RADIO		1
#define PLATFORM_HAS_BUTTON		1
#define PLATFORM_HAS_BATTERY		1
#define PLATFORM_HAS_TEMPERATURE	1
#define PLATFORM_HAS_ALTITUDE		1
#define PLATFORM_HAS_PRESSURE		1
#define PLATFORM_HAS_LIGHT		0


#define F_CPU				32000000ul		// HSE clock frequency
#define F_LSI				37000ul			// LSI clock frequency
#define F_LSE				32768ul			// LSE clock frequency

#define ENABLE_WATCHDOG			0			// Start watchdog
#define WATCHDOG_USE_IWDG		1			// Use independant watchdog
#define WATCHDOG_IWDG_TIMEOUT		250			// Independant watchdog timeout in milliseconds

#define MCU_WAKE_UP_TIME		3400			// Time in microseconds to wake up from low power mode


#define REDIRECT_STDIO_STRINGMODE	1
#define UART1_CONF_TX_WITH_INTERRUPT	0
#define WITH_SERIAL_LINE_INPUT		1
#define ENERGEST_CONF_ON		0
#define TELNETD_CONF_NUMLINES		6
#define NETSTACK_CONF_RADIO		lora_radio_driver
#define NETSTACK_RADIO_MAX_PAYLOAD_LEN	LORA_MAX_PAYLOAD_SIZE


/* Define ticks/second for slow and fast clocks. Notice that these should be a power
   of two, eg 64,128,256,512 etc, for efficiency as PoT's can be optimized well */
#define CLOCK_CONF_SECOND		128			// max 2048
#define RTIMER_ARCH_SECOND		8192			// max 8192
#define RTIMER_CLOCK_DIFF(a,b)		((signed short)((a)-(b)))
#ifndef CONTIKI3
  #define RTIMER_CLOCK_LT(a,b)		((signed short)((a)-(b)) < 0)
#endif
typedef unsigned long clock_time_t;
typedef unsigned long long rtimer_clock_t;


#define CC_CONF_REGISTER_ARGS		0
#define CC_CONF_FUNCTION_POINTER_ARGS	1
#define CC_CONF_FASTCALL
#define CC_CONF_VA_ARGS			1
#define CC_CONF_INLINE			inline

#define CCIF
#define CLIF


/* These names are deprecated, use C99 names */
typedef uint8_t		u8_t;
typedef uint16_t	u16_t;
typedef uint32_t	u32_t;
typedef int32_t 	s32_t;
typedef unsigned short	uip_stats_t;

/*---------------------------------------------------------------------------*/
#endif /* __PLATFORM_CONF_H__ */
/*---------------------------------------------------------------------------*/
