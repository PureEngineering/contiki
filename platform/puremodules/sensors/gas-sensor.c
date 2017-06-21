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
 *         MICS6814 (gas sensor) and ADS7924 (ADC) driver module for cc2650
 * \author
 *         Jon Steelsmith, Sashi Ono <info@pureengineering.com>
 */
/*---------------------------------------------------------------------------*/
#include "contiki-conf.h"
#include "lib/sensors.h"
#include "sys/ctimer.h"
#include "ti-lib.h"
#include "board-i2c.h"
#include "sensor-common.h"

#include "gas-sensor.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

static int TIME_BOOT; // time to boot
static int sensor_status = SENSOR_STATUS_DISABLED; //current status of sensor

static struct ctimer startup_timer;

static void notify_ready(void *not_used){
  sensor_status = SENSOR_STATUS_READY;
  sensors_changed(&gas_sensor); //contiki sensors api post "sensor changed" process
}

static void enable_sensor(bool enable){

  uint8_t mode;
  uint8_t pwr_contrl;

  //pwruptime [0..31] time = pwruptime x 2us;
  //i.e 10 x 2us = 20us before chip reads register
  uint8_t pwruptime = 0; //read right away

  /*
  TIME_BOOT = time before we can read from the sensor
  pwruptime * 2 = # of micro seconds before the chip converts
  /8 because every increment of TIME_BOOT is <8 micro seconds
  + 2 for a buffer (chip start up time and acquisition times)
  */
  TIME_BOOT = 2;
  TIME_BOOT += ((pwruptime * 2)/8);

  board_i2c_select(BOARD_I2C_INTERFACE_0, GAS_I2C_ADDRESS);
  if(enable){

    mode = 0xC8; // 1100 1000 - awake, manual-scan mode, channel 0 selected
    sensor_common_write_reg(MODE_CNTRL, &mode, sizeof(mode));
    pwr_contrl = 0x60; // 0110 0000 - active high, pin enabled
    pwr_contrl = pwr_contrl | pwruptime;
    sensor_common_write_reg(PWR_CONFIG, &pwr_contrl, sizeof(pwr_contrl));

  }
  else{
    pwr_contrl = 0x00; //disable powercon pin
    sensor_common_write_reg(PWR_CONFIG, &pwr_contrl, sizeof(pwr_contrl));
    mode = 0x00; // 0000 0000 - idle mode
    sensor_common_write_reg(MODE_CNTRL, &mode, sizeof(mode));
  }
}


static bool read_data(uint8_t *data, int type) {
  bool success;
  board_i2c_select(BOARD_I2C_INTERFACE_0, GAS_I2C_ADDRESS);

  if (type == GAS_OX){
    success = sensor_common_read_reg(DATA0_U, data, 2);
  }
  else if (type == GAS_NH3){
    success = sensor_common_read_reg(DATA1_U, data, 2);
  }
  else{
    success = sensor_common_read_reg(DATA2_U, data, 2);
  }
  if (!success) {
    dbg_gas("GAS - failed to read\n");
    sensor_common_set_error_data(data, 1);
  }

  return success;
}

/*
   return value of sensor depending on given type
*/

static int value(int type){
  int check;
  uint16_t ret;

  uint8_t sensor_reading[2] = {0}; //2 registers

  if(sensor_status != SENSOR_STATUS_READY){
    dbg_gas("GAS - Sensor not ready\n");
    return CC26XX_SENSOR_READING_ERROR;
  }
  if ((type != GAS_OX) && (type != GAS_NH3) && (type != GAS_RED)){
    dbg_gas("GAS - Invalid type\n");
    return CC26XX_SENSOR_READING_ERROR;
  }

  check = read_data(sensor_reading, type);

  if(!check){
    dbg_gas("GAS - Read error\n");
    return CC26XX_SENSOR_READING_ERROR;
  }
  dbg_gas("GAS - Value Read %x%x\n",sensor_reading[0],sensor_reading[1]);

  ret = (uint16_t)(((uint16_t)sensor_reading[0] << 8) | ((uint16_t)sensor_reading[1] >> 4));

  return (int)ret;
}

/**
 * \brief Configuration function for the LIS3MDL sensor.
 *
 * \param type Activate, enable or disable the sensor. See below
 * \param enable
 *
 * When type == SENSORS_HW_INIT we turn on the hardware
 * When type == SENSORS_ACTIVE and enable==1 we enable the sensor
 * When type == SENSORS_ACTIVE and enable==0 we disable the sensor
 */
static int configure(int type, int enable){
  switch (type){
    case SENSORS_HW_INIT:
      sensor_status = SENSOR_STATUS_INITIALISED;
      //initialize();
      //enable_sensor(0);
      break;
    case SENSORS_ACTIVE:
      if (sensor_status == SENSOR_STATUS_DISABLED){ //check to see if sensors is initalized
        return SENSOR_STATUS_DISABLED;
      }
      if(enable){
        dbg_gas("GAS - enabling sensor\n");
        enable_sensor(1);
        //calls notify ready function which tells main process that sensor is ready to be read
        dbg_gas("GAS - Boot time = %d micro seconds\n",TIME_BOOT*8);
        ctimer_set(&startup_timer, TIME_BOOT, notify_ready, NULL);
        sensor_status = SENSOR_STATUS_NOT_READY;
      } else {
        dbg_gas("GAS - disabling sensor\n");
        ctimer_stop(&startup_timer);
        enable_sensor(0);
        sensor_status = SENSOR_STATUS_INITIALISED;\
        dbg_gas("GAS - DISABLED\n");
      }
      break;
    default:
      break;
    }
  return sensor_status;
}


static int status(int type) {
  switch (type) {
  case SENSORS_ACTIVE:
  case SENSORS_READY:
    return sensor_status;
    break;
  default:
    break;
  }
  return SENSOR_STATUS_DISABLED;
}


//contikis api for sensors
//Every sensor must have a value, configure and status function
SENSORS_SENSOR(gas_sensor, "GAS", value, configure, status);
