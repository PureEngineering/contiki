/*
 * Copyright (c) 2016, National Security Technologies, LLC.
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
 *         BME280 driver for cc2650
 * \author
 *         Jon Steelsmith, Sashi Ono <info@pureengineering.com>
 */
/*---------------------------------------------------------------------------*/
#ifndef BME_280_SENSOR_H_
#define BME_280_SENSOR_H_
/*---------------------------------------------------------------------------*/
#define BME_280_SENSOR_TYPE_TEMP 1
#define BME_280_SENSOR_TYPE_PRESS 2
#define BME_280_SENSOR_TYPE_HUM 3

/*---------------------------------------------------------------------------*/
#define DEBUG_BME 1
#if DEBUG_BME
#define DBG_BME(...) printf(__VA_ARGS__)
#else
#define DBG_BME(...)
#endif

#define BME280_S32_t int32_t
#define BME280_U32_t uint32_t

/*---------------------------------------------------------------------------*/
#define BME280_I2C_ADDRESS 0x76
/*---------------------------------------------------------------------------*/
/* Registers */

// calibration data is stored in two seperate locations
#define ADDR_CALIB 0x88
#define ADDR_CALIB_HUM 0xA1

#define ADDR_PROD_ID 0xD0
#define ADDR_RESET 0xE0
#define ADDR_CTRL_HUM 0xF2
#define ADDR_STATUS 0xF3
#define ADDR_CTRL_MEAS 0xF4
#define ADDR_CONFIG 0xF5
#define ADDR_PRESS_MSB 0xF7
#define ADDR_PRESS_LSB 0xF8
#define ADDR_PRESS_XLSB 0xF9
#define ADDR_TEMP_MSB 0xFA
#define ADDR_TEMP_LSB 0xFB
#define ADDR_TEMP_XLSB 0xFC
#define ADDR_HUM_MSB 0xFD
#define ADDR_HUM_LSB 0xFE
/*---------------------------------------------------------------------------*/
/* Reset values */
#define VAL_PROD_ID 0x58
#define VAL_RESET 0x00
#define VAL_STATUS 0x00
#define VAL_CTRL_MEAS 0x00
#define VAL_CONFIG 0x00
#define VAL_PRESS_MSB 0x80
#define VAL_PRESS_LSB 0x00
#define VAL_TEMP_MSB 0x80
#define VAL_TEMP_LSB 0x00
/*---------------------------------------------------------------------------*/
/* Test values */
#define VAL_RESET_EXECUTE 0xB6
#define VAL_CTRL_MEAS_TEST 0x55
/*---------------------------------------------------------------------------*/
/* Misc. */
#define MEAS_DATA_SIZE 8

// calibration data is stores in two seperate locations
#define CALIB_DATA_SIZE 24
#define CALIB_DATA_SIZE_HUM 9
/*---------------------------------------------------------------------------*/
#define RES_OFF 0
#define RES_ULTRA_LOW_POWER 1
#define RES_LOW_POWER 2
#define RES_STANDARD 3
#define RES_HIGH 5
#define RES_ULTRA_HIGH 6
/*---------------------------------------------------------------------------*/
/* Bit fields in CTRL_MEAS register */
#define PM_OFF 0
#define PM_FORCED 1
#define PM_NORMAL 3
/*---------------------------------------------------------------------------*/
extern const struct sensors_sensor bme_280_sensor;
/*---------------------------------------------------------------------------*/
#endif /* BMP_280_SENSOR_H_ */
