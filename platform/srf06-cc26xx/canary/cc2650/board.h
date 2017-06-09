/*
 * Copyright (c) 2014, Texas Instruments Incorporated - http://www.ti.com/
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
/*---------------------------------------------------------------------------*/
/** \addtogroup cc26xx-srf-tag
 * @{
 *
 * \defgroup sensortag-cc26xx-peripherals Sensortag Peripherals
 *
 * Defines related to the CC2650 Sensortag
 *
 * This file provides connectivity information on LEDs, Buttons, UART and
 * other peripherals
 *
 * This file can be used as the basis to configure other boards using the
 * CC13xx/CC26xx code as their basis.
 *
 * This file is not meant to be modified by the user.
 * @{
 *
 * \file
 * Header file with definitions related to the I/O connections on the TI
 * Sensortag
 *
 * \note   Do not include this file directly. It gets included by contiki-conf
 *         after all relevant directives have been set.
 */
/*---------------------------------------------------------------------------*/
#ifndef BOARD_H_
#define BOARD_H_
/*---------------------------------------------------------------------------*/
#include "ioc.h"

/*---------------------------------------------------------------------------*/
/**
 * \name LED configurations
 *
 * Those values are not meant to be modified by the user
 * @{
 */

#undef LEDS_GREEN
#undef LEDS_YELLOW
#undef LEDS_RED
#undef LEDS_CONF_ALL

#define LED 1
#define LEDS_RED       LED
#define LEDS_GREEN     LED
#define LEDS_YELLOW    LED
#define LEDS_ORANGE    LED


/* Notify various examples that we have LEDs */

#define PLATFORM_HAS_LEDS        1

//Serialboot loader
#if ROM_BOOTLOADER_ENABLE
#define SET_CCFG_BL_CONFIG_BOOTLOADER_ENABLE            0xC5
#define SET_CCFG_BL_CONFIG_BL_LEVEL                     0x00 // active low
#define SET_CCFG_BL_CONFIG_BL_PIN_NUMBER                0x0B // dio 11
#define SET_CCFG_BL_CONFIG_BL_ENABLE                    0xC5
#else
#define SET_CCFG_BL_CONFIG_BOOTLOADER_ENABLE            0x00
#define SET_CCFG_BL_CONFIG_BL_LEVEL                     0x01
#define SET_CCFG_BL_CONFIG_BL_PIN_NUMBER                0xFF
#define SET_CCFG_BL_CONFIG_BL_ENABLE                    0xFF
#endif

/*
  hex value of pin
  i.e. pin 13 = IOID_13 = 0x0000000D
       pin 30 = IOID_30 = 0x0000001E

  IOID defined in contiki/cpu/cc26xx-cc13xx/lib/cc26xxware/driverlib/ioc.h
*/

#define BOARD_IOID_LIGHT_MAGNET_WAKEUP IOID_0
#define BOARD_IOID_AUDIO_VCC           IOID_1
#define BOARD_IOID_UART_RX             IOID_28
#define BOARD_IOID_UART_TX             IOID_3
#define BOARD_IOID_UART_CTS            IOID_UNUSED //must be defined but doesnt have to be used
#define BOARD_IOID_UART_RTS            IOID_UNUSED //must be defined but doesnt have to be used
#define BOARD_IOID_DIO4                IOID_4 //UNUSED
#define BOARD_IOID_SDA                 IOID_5
#define BOARD_IOID_SCL                 IOID_6
#define BOARD_IOID_SDA_HP              IOID_UNUSED //TI has two interfaces for I2C
#define BOARD_IOID_SCL_HP              IOID_UNUSED //Normal I2C is interface 0
#define BOARD_IOID_GPS_RESET           IOID_7
#define BOARD_IOID_AUDIO_CLOCK         IOID_8
#define BOARD_IOID_AUDIO_DATA          IOID_9
#define BOARD_IOID_GPS_ON_OFF          IOID_10
#define BOARD_IOID_BOOT                IOID_11
#define BOARD_IOID_GPS_1PPS            IOID_12
#define BOARD_IOID_ACCEL_INT2          IOID_13
#define BOARD_IOID_PIR_OUT             IOID_14
#define BOARD_IOID_LED                 IOID_15
#define BOARD_IOID_GPS_ENABLE          IOID_16 //TDO
#define BOARD_IOID_SPI_CLK_FLASH       IOID_17 //TDI called flash to match board-spi in common folder
#define BOARD_IOID_SPI_MISO            IOID_18
#define BOARD_IOID_SPI_MOSI            IOID_19
#define BOARD_IOID_BUZZER              IOID_21


//renamed radio pins to match sx1276 driver naming scheme
#define RADIO_NSS               IOID_20
#define RADIO_RESET             IOID_22 //match sx1276 driver pin name
#define RADIO_DIO_0             IOID_23
#define RADIO_DIO_1             IOID_24
#define RADIO_DIO_2             IOID_25
#define RADIO_DIO_3             IOID_26
#define RADIO_DIO_4             IOID_UNUSED
#define RADIO_DIO_5             IOID_27
#define RADIO_ANT_SWITCH_HF     IOID_UNUSED
#define RADIO_ANT_SWITCH_LF     IOID_UNUSED

#define BOARD_IOID_GPS_TX              IOID_28
#define BOARD_IOID_GPS_RX              IOID_29
#define BOARD_IOID_GPS_WAKEUP          IOID_30

/*--------------/
;    LEDs       ;
---------------*/
#define BOARD_LED                    BOARD_IOID_LED

/*--------------/
;    UART       ;
---------------*/
#define BOARD_UART_RX               (1 << BOARD_IOID_UART_RX)
#define BOARD_UART_TX               (1 << BOARD_IOID_UART_TX)
#define BOARD_UART_CTS              (1 << BOARD_IOID_UART_CTS)
#define BOARD_UART_RTS              (1 << BOARD_IOID_UART_RTS)

/*--------------/
;    I2C        ;
---------------*/
#define BOARD_SDA                   (1 << BOARD_IOID_SDA)
#define BOARD_SCL                   (1 << BOARD_IOID_SCL)

/*--------------/
;    SPI        ;
---------------*/
#define BOARD_SPI_MOSI               (1 << BOARD_IOID_SPI_MOSI)
#define BOARD_SPI_MISO               (1 << BOARD_IOID_SPI_MISO)

/*--------------/
;    BUZZER     ;
---------------*/
#define BOARD_BUZZER_INPUT                   (1 << BOARD_IOID_BUZZER_INPUT)




#define BOARD_STRING "CC2650 Canary"

/** @} */
/*---------------------------------------------------------------------------*/
#endif /* BOARD_H_ */
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 */
