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

#define LIS2DE12_I2C_ADDRESS       0x19
/*----------------------------------
              REGISTERS
----------------------------------*/
#define STATUS_REG_AUX_ADDR          0x07 // r
#define OUT_TEMP_L_ADDR              0x0C // r
#define OUT_TEMP_H_ADDR              0x0D // r
#define WHO_AM_I_ADDR                0x0F // r  default: 00110011
#define TEMP_CFG_REG_ADDR            0x1F // rw default: 00000000
#define CTRL_REG1_ADDR               0x20 // rw default: 00000111
#define CTRL_REG2_ADDR               0x21 // rw default: 00000000
#define CTRL_REG3_ADDR               0x22 // rw default: 00000000
#define CTRL_REG4_ADDR               0x23 // rw default: 00000000
#define CTRL_REG5_ADDR               0x24 // rw default: 00000000
#define CTRL_REG6_ADDR               0x25 // rw default: 00000000
#define REFERENCE_ADDR               0x26 // rw default: 00000000
#define STATUS_REG_ADDR              0x27 // r
#define OUT_X_ADDR                   0x29 // r
#define OUT_Y_ADDR                   0x2B // r
#define OUT_Z_ADDR                   0x2D // r
#define FIFO_CTRL_REG_ADDR           0x2E // rw default: 00000000
#define FIFO_SRC_REG_ADDR            0x2F // r
#define INT1_CFG_ADDR                0x30 // rw default: 00000000
#define INT1_SRC_ADDR                0x31 // r
#define INT1_THS_ADDR                0x32 // rw default: 00000000
#define INT1_DURATION_ADDR           0x33 // rw default: 00000000
#define INT2_CFG_ADDR                0x34 // rw default: 00000000
#define INT2_SRC_ADDR                0x35 // r
#define INT2_THS_ADDR                0x36 // rw default: 00000000
#define INT2_DURATION_ADDR           0x37 // rw default: 00000000
#define CLICK_CFG_ADDR               0x38 // rw default: 00000000
#define CLICK_SRC_ADDR               0x39 // r
#define CLICK_THS_ADDR               0x3A // rw default: 00000000
#define TIME_LIMIT_ADDR              0x3B // rw default: 00000000
#define TIME_LATENCY_ADDR            0x3C // rw default: 00000000
#define TIME_WINDOW_ADDR             0x3D // rw default: 00000000
#define ACT_THS_ADDR                 0x3E // rw default: 00000000
#define ACT_DUR_ADDR                 0x3F // rw default: 00000000

/*----------------------------------
              TIMINGS
----------------------------------*/
#define TIME_BOOT                    5 // 5 ticks

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
      //initialize();
      //enable_sensor(0);
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
