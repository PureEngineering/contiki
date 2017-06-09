//
// Created by furqaan on 5/5/17.
//

#include "mag-sensor.h"
#include "lib/sensors.h"
#include "board-i2c.h"

static void turnOn(void);

static void init(void) {
    board_i2c_select(BOARD_I2C_INTERFACE_0, 0x1C);
    uint8_t config;
    config = 0x14;
    sensor_common_write_reg(0x20, &config, 1);
}

static void turnOn(void) {
    uint8_t config;
    config = 0x21;
    sensor_common_write_reg(0x22, &config, 1);
}

static void shutDown(void) {
    uint8_t config;
    config = 0x23;
    sensor_common_write_reg(0x22, &config, 1);
}

static uint16_t getX(void) {
    uint16_t data;
    if(!sensor_common_read_reg(0x28, &data, 2))
        return CC26XX_SENSOR_READING_ERROR;
    return data;
}

static int getY(void) {
    uint16_t data;
    sensor_common_read_reg(0x2A, &data, 2);
    return data;
}

static int getZ(void) {
    uint16_t data;
    sensor_common_read_reg(0x2C, &data, 2);
    return data;
}

static int value(mag_field field) {
    switch (field) {
        case X:
            return getX();
        case Y:
            return getY();
        case Z:
            return getZ();
    }
    return 0;
}

static int configure(int type, int enable){
    switch(type){
        case SENSORS_HW_INIT:
            init();
            break;
        case SENSORS_ACTIVE:
            if(enable) {
                turnOn();
            }else {
                shutDown();
            }
            break;
    }
    return 0;
}

static int status(int type) {
    return 0;
}

SENSORS_SENSOR(mag_sensor, "LIS3MDL", value, configure, status);

