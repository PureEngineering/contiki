/*
 * Copyright (c) 2016, Pure Engineering LLC.
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
 */

/**
 * \file
 *         Support file to use SEMTECH's LoRa Radio driver files for STM32L151C8
 *         based boards with TI's cc2650 mcu.
 * \author
 *         Jon Steelsmith, Sashi Ono <info@pureengineering.com>
 */
#ifndef __LORA_SX1276_SUPPORT_H__
#define __LORA_SX1276_SUPPORT_H__

#include <stdint.h>

#include "sys/ctimer.h"

#define DelayMs(x)    clock_delay_usec(x * 100)

#define __IO volatile

//Start GPIO
typedef enum
{
    PIN_INPUT = 0,
    PIN_OUTPUT,
    PIN_ALTERNATE_FCT,
    PIN_ANALOGIC
}PinModes;

typedef enum
{
    PIN_NO_PULL = 0,
    PIN_PULL_UP,
    PIN_PULL_DOWN
}PinTypes;

typedef enum
{
    PIN_PUSH_PULL = 0,
    PIN_OPEN_DRAIN
}PinConfigs;

typedef enum
{
    NO_IRQ = 0,
    IRQ_RISING_EDGE,
    IRQ_FALLING_EDGE,
    IRQ_RISING_FALLING_EDGE
}IrqModes;

typedef enum
{
    IRQ_VERY_LOW_PRIORITY = 0,
    IRQ_LOW_PRIORITY,
    IRQ_MEDIUM_PRIORITY,
    IRQ_HIGH_PRIORITY,
    IRQ_VERY_HIGH_PRIORITY
}IrqPriorities;

typedef struct
{
    int  pin;
    uint16_t pinIndex;
    void *port;
    uint16_t portIndex;
}Gpio_t;

typedef void( GpioIrqHandler )( uint8_t );

void GpioInit( Gpio_t *obj, int pin, PinModes mode, PinConfigs config, PinTypes type, uint32_t value );

void GpioSetInterrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler );

void GpioRemoveInterrupt( Gpio_t *obj );

void GpioWrite( Gpio_t *obj, uint32_t value );

uint32_t GpioRead( Gpio_t *obj );

//Start Spi
struct Spi_s
{
    //SPI_TypeDef *Spi;
    Gpio_t Mosi;
    Gpio_t Miso;
    Gpio_t Sclk;
    Gpio_t Nss;
    uint32_t freq;
};

typedef struct Spi_s Spi_t;

uint16_t SpiInOut( Spi_t *obj, uint16_t outData );
//End Spi

//Start Timer
typedef struct TimerEvent_s
{
    uint32_t Timestamp;         //! Current timer value
    uint32_t ReloadValue;       //! Timer delay value
    bool IsRunning;             //! Is the timer currently running
    void ( *Callback )( void *); //! Timer IRQ callback function
    struct TimerEvent_s *Next;  //! Pointer to the next Timer object.
    struct ctimer timer;
}TimerEvent_t;

void TimerInit( TimerEvent_t *obj, void ( *callback )( void *) );

void TimerStart( TimerEvent_t *obj );

void TimerStop( TimerEvent_t *obj );

void TimerSetValue( TimerEvent_t *obj, uint32_t value );
//End Timer


#endif
