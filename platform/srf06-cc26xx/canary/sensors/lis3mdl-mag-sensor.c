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
 *         lis3mdl (magnetometer) driver for cc2650
 * \author
 *         Jon Steelsmith, Sashi Ono <info@pureengineering.com>
 */
/*---------------------------------------------------------------------------*/
#include "contiki-conf.h"
#include "lib/sensors.h"
#include "lis3mdl-mag-sensor.h"
#include "sys/ctimer.h"
#include "ti-lib.h"
#include "board-i2c.h"
#include "sensor-common.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

static int sensor_status = SENSOR_STATUS_DISABLED; //current status of sensor

static struct ctimer startup_timer;

static void notify_ready(void *not_used){
  sensor_status = SENSOR_STATUS_READY;
  sensors_changed(&lis3mdl_mag_sensor); //contiki sensors api post "sensor changed" process
}

static void enable_sensor(bool enable){

  uint8_t reg1_config;
  uint8_t reg2_config;
  uint8_t reg3_config;
  uint8_t reg4_config;
  uint8_t reg5_config;
  uint8_t whoami;

  if(enable){
    board_i2c_select(BOARD_I2C_INTERFACE_0, LIS3MDL_I2C_ADDRESS);
    sensor_common_read_reg(WHO_AM_I_ADDR, &whoami, sizeof(whoami));
    dbg_mag("LIS3MDL - who am i %x\n", whoami);
    reg1_config = 0x14; // 0001 0100 - normal mode 20hz, low power mode
    sensor_common_write_reg(CTRL_REG1_ADDR, &reg1_config, sizeof(reg1_config));
    reg2_config = 0x00; // 0000 0000 - +- 4 gauss
    sensor_common_write_reg(CTRL_REG2_ADDR, &reg2_config, sizeof(reg2_config));
    reg3_config = 0x21; // 0010 0000 - low power mode **overides ctrl reg1 10hz to .625hz**  single
    sensor_common_write_reg(CTRL_REG3_ADDR, &reg3_config, sizeof(reg3_config));
    reg4_config = 0x00; // 0000 0000 - z low power mode, lsb in lower address
    sensor_common_write_reg(CTRL_REG4_ADDR, &reg4_config, sizeof(reg4_config));
    reg5_config = 0x00; // 0000 0000 - fast read disabled continuous update
    sensor_common_write_reg(CTRL_REG5_ADDR, &reg5_config, sizeof(reg5_config));
  }
  else{
    reg3_config = 0x23; // 0010 0011 - shutdown mode
    sensor_common_write_reg(CTRL_REG3_ADDR, &reg3_config, sizeof(reg3_config));
  }
}


static bool read_data(uint8_t *data, int type) {
  bool success;
  board_i2c_select(BOARD_I2C_INTERFACE_0, LIS3MDL_I2C_ADDRESS);

  if (type == MAG_X){
    success = sensor_common_read_reg(OUT_X_L_ADDR, data, 2);
  }
  else if (type == MAG_Y){
    success = sensor_common_read_reg(OUT_Y_L_ADDR, data, 2);
  }
  else{
    success = sensor_common_read_reg(OUT_Z_L_ADDR, data, 2);
  }
  if (!success) {
    dbg_mag("LIS3MDL - failed to read\n");
    sensor_common_set_error_data(data, 1);
  }

  return success;
}

/*
   return value of sensor depending on given type
*/

static int value(int type){
  int check;
  uint8_t sensor_reading[2] = {0};
  uint8_t *data = sensor_reading;

  if(sensor_status != SENSOR_STATUS_READY){
    dbg_mag("LIS3MDL - Sensor not ready\n");
    return CC26XX_SENSOR_READING_ERROR;
  }
  if ((type != MAG_X) && (type != MAG_Y) && (type != MAG_Z)){
    dbg_mag("LIS3MDL - Invalid type\n");
    return CC26XX_SENSOR_READING_ERROR;
  }

  check = read_data(data, type);

  if(!check){
    dbg_mag("LIS3MDL - Read error\n");
    return CC26XX_SENSOR_READING_ERROR;
  }

  int16_t ret = (int16_t)(((uint16_t)sensor_reading[1] << 8) | ((uint16_t)sensor_reading[0]));

  dbg_mag("LIS3MDL - value read %x %x\n",sensor_reading[0],sensor_reading[1]);

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
      ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_MAG_INT);
      ti_lib_ioc_port_configure_set(BOARD_IOID_MAG_INT,IOC_PORT_GPIO,IOC_IOPULL_DOWN);
      break;
    case SENSORS_ACTIVE:
      if (sensor_status == SENSOR_STATUS_DISABLED){ //check to see if sensors is initalized
        return SENSOR_STATUS_DISABLED;
      }
      if(enable){
        dbg_mag("LIS3MDL - enabling sensor\n");
        enable_sensor(1);
        //calls notify ready function which tells main process that sensor is ready to be read
        ctimer_set(&startup_timer, TIME_BOOT, notify_ready, NULL);
        sensor_status = SENSOR_STATUS_NOT_READY;
      } else {
        dbg_mag("LIS3MDL - disabling sensor\n");
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
SENSORS_SENSOR(lis3mdl_mag_sensor, "LIS3MDL", value, configure, status);
