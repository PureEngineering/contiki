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
/**
 * \addtogroup sensortag-cc26xx-bmp-sensor
 * @{
 *
 * \file
 *  Driver for the Sensortag-CC26XX BMP280 Altimeter / Pressure Sensor
 */
/*---------------------------------------------------------------------------*/
#include "bme-280-sensor.h"
#include "board-i2c.h"
#include "contiki-conf.h"
#include "lib/sensors.h"
#include "sensor-common.h"
#include "sys/ctimer.h"
#include "ti-lib.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
/*---------------------------------------------------------------------------*/
#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
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
#define OSRST(v) ((v) << 5)
#define OSRSP(v) ((v) << 2)
/*---------------------------------------------------------------------------*/
typedef struct bme_280_calibration {
  uint16_t dig_T1;
  int16_t dig_T2;
  int16_t dig_T3;
  uint16_t dig_P1;
  int16_t dig_P2;
  int16_t dig_P3;
  int16_t dig_P4;
  int16_t dig_P5;
  int16_t dig_P6;
  int16_t dig_P7;
  int16_t dig_P8;
  int16_t dig_P9;
  uint8_t dig_H1;
  int16_t dig_H2;
  uint8_t dig_H3;
  int16_t dig_H4;
  int16_t dig_H5;
  int8_t dig_H6;
} bme_280_calibration_t;


/*---------------------------------------------------------------------------*/

// 36 total 8 bit Registers
// 18 total 16 bit registers
static uint8_t calib[CALIB_DATA_SIZE + CALIB_DATA_SIZE_HUM];
// bme_280_calibration_t *cal = (bme_280_calibration_t *)calib;
  bme_280_calibration_t cal;
/*---------------------------------------------------------------------------*/
#define SENSOR_STATUS_DISABLED 0
#define SENSOR_STATUS_INITIALISED 1
#define SENSOR_STATUS_NOT_READY 2
#define SENSOR_STATUS_READY 3

static int enabled = SENSOR_STATUS_DISABLED;
/*---------------------------------------------------------------------------*/
/* A buffer for the raw reading from the sensor */
#define SENSOR_DATA_BUF_SIZE 8

static uint8_t sensor_value[3]; // max of three registers per type of data
/*---------------------------------------------------------------------------*/
/* Wait SENSOR_STARTUP_DELAY clock ticks for the sensor to be ready - ~80ms */
#define SENSOR_STARTUP_DELAY 3

static struct ctimer startup_timer;
/*---------------------------------------------------------------------------*/
static void notify_ready(void *not_used) {
  enabled = SENSOR_STATUS_READY;
  sensors_changed(&bme_280_sensor);
}
/*---------------------------------------------------------------------------*/
static void select_on_bus(void) {
  /* Set up I2C */
  board_i2c_select(BOARD_I2C_INTERFACE_0, BME280_I2C_ADDRESS);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Initalise the sensor
 */
static void init(void) {
  uint8_t val;

  select_on_bus();

  /* Read and store calibration data */
  if (!sensor_common_read_reg(ADDR_CALIB, calib, CALIB_DATA_SIZE)) {
    printf("error reading calibration\n");
  }

  // read remaining values
  if (!sensor_common_read_reg(ADDR_CALIB_HUM, calib + CALIB_DATA_SIZE,
                              CALIB_DATA_SIZE_HUM)) {
    printf("error reading humidity calibration\n");
  }

  cal.dig_T1 = (uint16_t)(((uint16_t)calib[1] << 8) | (uint16_t)calib[0]); //unsigned
  cal.dig_T2 = (int16_t)(((uint16_t)calib[3] << 8) | (uint16_t)calib[2]);
  cal.dig_T3 = (int16_t)(((uint16_t)calib[5] << 8) | (uint16_t)calib[4]);
  cal.dig_P1 = (uint16_t)(((uint16_t)calib[7] << 8) | (uint16_t)calib[6]); // unsigned
  cal.dig_P2 = (int16_t)(((uint16_t)calib[9] << 8) | (uint16_t)calib[8]);
  cal.dig_P3 = (int16_t)(((uint16_t)calib[11] << 8) | (uint16_t)calib[10]);
  cal.dig_P4 = (int16_t)(((uint16_t)calib[13] << 8) | (uint16_t)calib[12]);
  cal.dig_P5 = (int16_t)(((uint16_t)calib[15] << 8) | (uint16_t)calib[14]);
  cal.dig_P6 = (int16_t)(((uint16_t)calib[17] << 8) | (uint16_t)calib[16]);
  cal.dig_P7 = (int16_t)(((uint16_t)calib[19] << 8) | (uint16_t)calib[18]);
  cal.dig_P8 = (int16_t)(((uint16_t)calib[21] << 8) | (uint16_t)calib[20]);
  cal.dig_P9 = (int16_t)(((uint16_t)calib[23] << 8) | (uint16_t)calib[22]);
  cal.dig_H1 = (uint8_t)calib[24];                               //unsigned char
  cal.dig_H2 = (int16_t)(((uint16_t)calib[26] << 8) | (uint16_t)calib[25]);
  cal.dig_H3 = (uint8_t)calib[27];                               //unsigned char
  cal.dig_H4 = (int16_t)(((uint16_t)calib[29] << 8) | (uint16_t)calib[28]);
  cal.dig_H5 = (int16_t)(((uint16_t)calib[31] << 8) | (uint16_t)calib[30]);
  cal.dig_H6 = (int8_t)calib[32];                                 //signed char

  /* Reset the sensor */
  val = VAL_RESET_EXECUTE;
  sensor_common_write_reg(ADDR_RESET, &val, sizeof(val));
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Enable/disable measurements
 * \param enable 0: disable, enable otherwise
 *
 * @return      none
 */
static void enable_sensor(bool enable) {
  uint8_t ctrl_hum;
  uint8_t ctrl_meas;
  uint8_t config;

  select_on_bus();


  if (enable) {
    ctrl_hum = 0x07; // 0000 0001 - oversampling humidty x1
    sensor_common_write_reg(ADDR_CTRL_HUM, &ctrl_hum, sizeof(ctrl_hum));
    ctrl_meas = 0xFD; // 0010 0101 - oversampling temp/pres x1 forced mode
    sensor_common_write_reg(ADDR_CTRL_MEAS, &ctrl_meas, sizeof(ctrl_meas));
    config = 0x00; // 0000 0000 - filter off
    sensor_common_write_reg(ADDR_CONFIG, &config, sizeof(config));
  } else {
    ctrl_meas = 0x00; // 0000 0000 - sleep mode
    sensor_common_write_reg(ADDR_CTRL_MEAS, &ctrl_meas, sizeof(ctrl_meas));
  }


}
/*---------------------------------------------------------------------------*/
/**
 * \brief Read temperature, pressure, humidity data
 * \param data Pointer to a buffer where temperature and pressure will be
 *             written (8 bytes)
 * \return True if valid data could be retrieved
 */
static bool read_data(uint8_t *data, int type) {
  bool success;

  select_on_bus();

  if(type == BME_280_SENSOR_TYPE_TEMP){
    success = sensor_common_read_reg(ADDR_TEMP_MSB, data, 3); //read three temp registers
  } else if (type == BME_280_SENSOR_TYPE_PRESS){
    success = sensor_common_read_reg(ADDR_PRESS_MSB, data, 3); //read pressure
  } else if (type == BME_280_SENSOR_TYPE_HUM){
    success = sensor_common_read_reg(ADDR_HUM_MSB, data, 2); //humidity only has two registers
  }
  if (!success) {
    sensor_common_set_error_data(data, MEAS_DATA_SIZE);
  }

  return success;
}

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123”
// equals 51.23 DegC.
// t_fine carries fine temperature as global value
BME280_S32_t t_fine;
BME280_S32_t BME280_compensate_T_int32(BME280_S32_t adc_T) {


  BME280_S32_t var1, var2, T;
  var1 = ((((adc_T >> 3)-((BME280_S32_t)cal.dig_T1<<1)))*
          ((BME280_S32_t)cal.dig_T2))>>11;
  var2 = (((((adc_T >> 4) - ((BME280_S32_t)cal.dig_T1)) *
            ((adc_T >> 4) - ((BME280_S32_t)cal.dig_T1))) >>12) *
          ((BME280_S32_t)cal.dig_T3)) >>14;
  t_fine = var1 + var2;
  T = (t_fine * 5 + 128) >> 8;
  return T;
}
// Returns pressure in Pa as unsigned 32 bit integer. Output value of “96386”
// equals 96386 Pa = 963.86 hPa
BME280_U32_t BME280_compensate_P_int32(BME280_S32_t adc_P) {

  BME280_S32_t var1, var2;
  BME280_U32_t p;
  var1 = (((BME280_S32_t)t_fine) >> 1) - (BME280_S32_t)64000;
  var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * ((BME280_S32_t)cal.dig_P6);
  var2 = var2 + ((var1 * ((BME280_S32_t)cal.dig_P5)) << 1);
  var2 = (var2 >> 2) + (((BME280_S32_t)cal.dig_P4) << 16);
  var1 = (((cal.dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) +
          ((((BME280_S32_t)cal.dig_P2) * var1) >> 1)) >> 18;
  var1 = ((((32768 + var1)) * ((BME280_S32_t)cal.dig_P1)) >> 15);
  if (var1 == 0) {
    return 0; // avoid exception caused by division by zero
  }
  p = (((BME280_U32_t)(((BME280_S32_t)1048576) - adc_P) - (var2 >> 12))) * 3125;
  if (p < 0x80000000) {
    p = (p << 1) / ((BME280_U32_t)var1);
  } else {
    p = (p / (BME280_U32_t)var1) * 2;
  }
  var1 = (((BME280_S32_t)cal.dig_P9) *
          ((BME280_S32_t)(((p >> 3) * (p >> 3)) >> 13))) >>
         12;
  var2 = (((BME280_S32_t)(p >> 2)) * ((BME280_S32_t)cal.dig_P8)) >> 13;
  p = (BME280_U32_t)((BME280_S32_t)p + ((var1 + var2 + cal.dig_P7) >> 4));
  return p;
}

// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22
// integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH
BME280_U32_t bme280_compensate_H_int32(BME280_S32_t adc_H) {

  BME280_S32_t v_x1_u32r;
  v_x1_u32r = (t_fine - ((BME280_S32_t)76800));
  v_x1_u32r = (((((adc_H << 14) - (((BME280_S32_t)cal.dig_H4) << 20) - (
          ((BME280_S32_t)cal.dig_H5) * v_x1_u32r)) + ((BME280_S32_t)16384))
          >> 15) * (((((((v_x1_u32r * ((BME280_S32_t)cal.dig_H6)) >> 10) *
          (((v_x1_u32r * ((BME280_S32_t)cal.dig_H3)) >> 11) + ((BME280_S32_t)32768))) >>
          10) + ((BME280_S32_t)2097152)) * ((BME280_S32_t)cal.dig_H2) + 8192) >>  14));

  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
          ((BME280_S32_t)cal.dig_H1)) >>  4));

  v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
  v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
  return (BME280_U32_t)(v_x1_u32r >> 12);
}

static int value(int type) {
  int rv;
  int32_t temp = 0;
  uint32_t pres = 0;
  uint32_t hum = 0;

  if (enabled != SENSOR_STATUS_READY) {
    PRINTF("Sensor disabled or starting up (%d)\n", enabled);
    return CC26XX_SENSOR_READING_ERROR;
  }

  if ((type != BME_280_SENSOR_TYPE_TEMP) && type != BME_280_SENSOR_TYPE_PRESS &&
      type != BME_280_SENSOR_TYPE_HUM) {
    PRINTF("Invalid type\n");
    return CC26XX_SENSOR_READING_ERROR;
  } else {
    memset(sensor_value, 0, SENSOR_DATA_BUF_SIZE);

    rv = read_data(sensor_value, type);

    if (rv == 0) {
      PRINTF("exiting with reading error\n");
      return CC26XX_SENSOR_READING_ERROR;
    }

    if (type == BME_280_SENSOR_TYPE_TEMP) {

      int32_t t = (int32_t)((((uint32_t)(sensor_value[0])) << 12) |
                            ((uint32_t)(sensor_value[1]) << 4 ) | (uint32_t)sensor_value[2] >> 4);
      // calculate temperature

      temp = BME280_compensate_T_int32(t);

      rv = (int)temp;

    } else if (type == BME_280_SENSOR_TYPE_PRESS) {

      int32_t p = (int32_t)((((uint32_t)(sensor_value[0])) << 12) |
                            ((uint32_t)(sensor_value[1]) << 4 ) | (uint32_t)sensor_value[2]>>4);

      pres = BME280_compensate_P_int32(p);
      rv = (int)pres;
    } else if (type == BME_280_SENSOR_TYPE_HUM) {
      int32_t h = (int32_t)((((uint32_t)(sensor_value[0])) << 8) |
                            ((uint32_t)(sensor_value[1])));

      hum = bme280_compensate_H_int32(h);
      rv = (int)hum;
    }
  }

  return rv;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Configuration function for the BMP280 sensor.
 *
 * \param type Activate, enable or disable the sensor. See below
 * \param enable
 *
 * When type == SENSORS_HW_INIT we turn on the hardware
 * When type == SENSORS_ACTIVE and enable==1 we enable the sensor
 * When type == SENSORS_ACTIVE and enable==0 we disable the sensor
 */
static int configure(int type, int enable) {
  switch (type) {
  case SENSORS_HW_INIT:
    enabled = SENSOR_STATUS_INITIALISED;
    init();
    enable_sensor(0);
    break;
  case SENSORS_ACTIVE:
    /* Must be initialised first */
    if (enabled == SENSOR_STATUS_DISABLED) {
      return SENSOR_STATUS_DISABLED;
    }
    if (enable) {
      enable_sensor(1);
      ctimer_set(&startup_timer, SENSOR_STARTUP_DELAY, notify_ready, NULL);
      enabled = SENSOR_STATUS_NOT_READY;
    } else {
      ctimer_stop(&startup_timer);
      enable_sensor(0);
      enabled = SENSOR_STATUS_INITIALISED;
    }
    break;
  default:
    break;
  }
  return enabled;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Returns the status of the sensor
 * \param type SENSORS_ACTIVE or SENSORS_READY
 * \return 1 if the sensor is enabled
 */
static int status(int type) {
  switch (type) {
  case SENSORS_ACTIVE:
  case SENSORS_READY:
    return enabled;
    break;
  default:
    break;
  }
  return SENSOR_STATUS_DISABLED;
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(bme_280_sensor, "BME280", value, configure, status);
/*---------------------------------------------------------------------------*/
/** @} */
