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
 *         lis2de12 driver (acceleration) for cc2650
 * \author
 *         Jon Steelsmith, Sashi Ono <info@pureengineering.com>
 */
/*---------------------------------------------------------------------------*/
#include "contiki-conf.h"
#include "lib/sensors.h"
#include "lis2de12-accel-sensor.h"
#include "sys/ctimer.h"
#include "ti-lib.h"
#include "board-i2c.h"
#include "sensor-common.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define debug_accel 0
#if debug_accel
#define dbg_accel(...) printf(__VA_ARGS__)
#else
#define dbg_accel(...)
#endif

#define SENSOR_STATUS_DISABLED 0
#define SENSOR_STATUS_INITIALISED 1
#define SENSOR_STATUS_NOT_READY 2
#define SENSOR_STATUS_READY 3

static int sensor_status = SENSOR_STATUS_DISABLED; //current status of sensor
static struct ctimer startup_timer; //timer for starting up

/*----------------------------------
              Functions
----------------------------------*/
static void notify_ready(void *not_used){
  sensor_status = SENSOR_STATUS_READY;
  sensors_changed(&lis2de12_accel_sensor); //contiki sensors api post "sensor changed" process
}

static void enable_sensor(bool enable){

  uint8_t reg1_config;
  uint8_t reg2_config;
  uint8_t reg3_config;
  uint8_t reg4_config;
  uint8_t reg5_config;
  uint8_t reg6_config;
  uint8_t int1_cfg_config;
  uint8_t int1_ths_config;
  uint8_t int1_duration_config;

  if(enable){
    board_i2c_select(BOARD_I2C_INTERFACE_0, LIS2DE12_I2C_ADDRESS);
    reg1_config = 0x57; // 0101 0111 - normal mode 100hz, xyz enable
    sensor_common_write_reg(CTRL_REG1_ADDR, &reg1_config, sizeof(reg1_config));
    reg2_config = 0x01; // 0000 1001 - High pass filter turned on, interrupt 1 filter enabled
    sensor_common_write_reg(CTRL_REG2_ADDR, &reg2_config, sizeof(reg2_config));
    reg3_config = 0x00; // 0000 0000 - int1 pin enabling
    sensor_common_write_reg(CTRL_REG3_ADDR, &reg3_config, sizeof(reg3_config));
    reg4_config = 0x00; // 0000 0000 - continuous update, 2g scale selection, self test off
    sensor_common_write_reg(CTRL_REG4_ADDR, &reg4_config, sizeof(reg4_config));
    reg5_config = 0x08; // 0000 1000 - latch interrupt enabled on int1
    sensor_common_write_reg(CTRL_REG5_ADDR, &reg5_config, sizeof(reg5_config));
    reg6_config = 0x42; // 0100 0001 int1 function enabled for int2 pin, low active
    sensor_common_write_reg(CTRL_REG6_ADDR, &reg6_config, sizeof(reg6_config));
    int1_ths_config = 0x00; // all int1 registers need to be configured for interrupt
    sensor_common_write_reg(INT1_THS_ADDR, &int1_ths_config, sizeof(int1_ths_config));
    int1_cfg_config = 0x00;
    sensor_common_write_reg(INT1_CFG_ADDR, &int1_cfg_config, sizeof(int1_cfg_config));
    int1_duration_config = 0x00;
    sensor_common_write_reg(INT1_DURATION_ADDR, &int1_duration_config, sizeof(int1_duration_config));

  }
  else{
    reg1_config = 0x00;
    sensor_common_write_reg(CTRL_REG1_ADDR, &reg1_config, sizeof(reg1_config));
  }
}


static bool read_data(uint8_t *data, int type) {
  bool success;
  board_i2c_select(BOARD_I2C_INTERFACE_0, LIS2DE12_I2C_ADDRESS);

  if (type == ACCEL_X){
    success = sensor_common_read_reg(OUT_X_ADDR, data, 1);
  }
  else if (type == ACCEL_Y){
    success = sensor_common_read_reg(OUT_Y_ADDR, data, 1);
  }
  else{
    success = sensor_common_read_reg(OUT_Z_ADDR, data, 1);
  }
  if (!success) {
    dbg_accel("LIS2DE12 - failed to read\n");
    sensor_common_set_error_data(data, 1);
  }

  return success;
}

/*
   return value of sensor depending on given type
*/

static int value(int type){
  int check;
  uint8_t sensor_reading = 0;

  if(sensor_status != SENSOR_STATUS_READY){
    dbg_accel("LIS2DE12 - Sensor not ready\n");
    return CC26XX_SENSOR_READING_ERROR;
  }
  if ((type != ACCEL_X) && (type != ACCEL_Y) && (type != ACCEL_Z)){
    dbg_accel("LIS2DE12 - Invalid type\n");
    return CC26XX_SENSOR_READING_ERROR;
  }

  check = read_data(&sensor_reading, type);
  int8_t ret = sensor_reading;
  if(!check){
    dbg_accel("LIS2DE12 - Read error\n");
    return CC26XX_SENSOR_READING_ERROR;
  }

  dbg_accel("LIS2DE12 - value read %x\n",sensor_reading);

  return (int)ret;
}

/**
 * \brief Configuration function for the lis2de12 sensor.
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
      ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_ACCEL_INT2);
      ti_lib_ioc_port_configure_set(BOARD_IOID_ACCEL_INT2,IOC_PORT_GPIO,IOC_IOPULL_DOWN);
      break;
    case SENSORS_ACTIVE:
      if (sensor_status == SENSOR_STATUS_DISABLED){ //check to see if sensors is initalized
        return SENSOR_STATUS_DISABLED;
      }
      if(enable){
        dbg_accel("LIS2DE12 - enabling sensor\n");
        enable_sensor(1);
        //calls notify ready function which tells main process that sensor is ready to be read
        ctimer_set(&startup_timer, TIME_BOOT, notify_ready, NULL);
        sensor_status = SENSOR_STATUS_NOT_READY;
      } else {
        dbg_accel("LIS2DE12 - disabling sensor\n");
        ctimer_stop(&startup_timer);
        enable_sensor(0);
        sensor_status = SENSOR_STATUS_INITIALISED;
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
SENSORS_SENSOR(lis2de12_accel_sensor, "LIS2DE12", value, configure, status);
