#ifndef GAS_SENSOR_H
#define GAS_SENSOR_H

#include <stdio.h>

#define debug_gas 0
#if debug_gas
#define dbg_gas(...) printf(__VA_ARGS__)
#else
#define dbg_gas(...)
#endif

#define GAS_OX                         1
#define GAS_NH3                        2
#define GAS_RED                        3

#define GAS_I2C_ADDRESS                0x48 //1001000 A0 PIN GROUNDED

#define MODE_CNTRL                     0x00
#define INT_CNTRL                      0x01
#define DATA0_U                        0x02
#define DATA0_L                        0x03
#define DATA1_U                        0x04
#define DATA1_L                        0x05
#define DATA2_U                        0x06
#define DATA2_L                        0x07
#define DATA3_U                        0x08
#define DATA3_L                        0x09
#define ULR0                           0x0A
#define LLR0                           0x0B
#define ULR1                           0x0C
#define LLR1                           0x0D
#define ULR2                           0x0E
#define LLR2                           0x0F
#define ULR3                           0x10
#define LLR3                           0x11
#define INT_CONFIG                     0x12
#define SLP_CONFIG                     0x13
#define ACQ_CONFIG                     0x14
#define PWR_CONFIG                     0x15
#define RESET                          0X16

#define SENSOR_STATUS_DISABLED         0
#define SENSOR_STATUS_INITIALISED      1
#define SENSOR_STATUS_NOT_READY        2
#define SENSOR_STATUS_READY            3


extern const struct sensors_sensor gas_sensor;

#endif
