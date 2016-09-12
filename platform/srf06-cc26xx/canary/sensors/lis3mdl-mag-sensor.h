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
#ifndef LIS3MDL_MAG_SENSOR_H
#define LIS3MDL_MAG_SENSOR_H

#include <stdio.h>

#define debug_mag 0
#if debug_mag
#define dbg_mag(...) printf(__VA_ARGS__)
#else
#define dbg_mag(...)
#endif

//types for reading values
#define MAG_X   1
#define MAG_Y   2
#define MAG_Z   3

#define LIS3MDL_I2C_ADDRESS            0x1C

//registers
#define WHO_AM_I_ADDR                  0x0F // r default: 0011 1101
#define CTRL_REG1_ADDR                 0x20 // rw default: 0001 0000
#define CTRL_REG2_ADDR                 0x21 // rw default: 0000 0000
#define CTRL_REG3_ADDR                 0x22 // rw default: 0000 0011
#define CTRL_REG4_ADDR                 0x23 // rw default: 0000 0000
#define CTRL_REG5_ADDR                 0x24 // rw default: 0000 0000
#define STATUS_REG_ADDR                0x27 // r default: OUTPUT
#define OUT_X_L_ADDR                   0x28 // r default: OUTPUT
#define OUT_X_H_ADDR                   0x29 // r default: OUTPUT
#define OUT_Y_L_ADDR                   0x2A // r default: OUTPUT
#define OUT_Y_H_ADDR                   0x2B // r default: OUTPUT
#define OUT_Z_L_ADDR                   0x2C // r default: OUTPUT
#define OUT_Z_H_ADDR                   0x2D // r default: OUTPUT
#define TEMP_OUT_L_ADDR                0x2E // r default: OUTPUT
#define TEMP_OUT_H_ADDR                0x2F // r default: OUTPUT
#define INT_CFG_ADDR                   0x30 // rw default: 0000 0000
#define INT_SRC_ADDR                   0x31 // r default: 0000 0000
#define INT_THS_L_ADDR                 0x32 // r default: 0000 0000
#define INT_THS_H_ADDR                 0x33 // r default: 0000 0000

//error messages
#define SENSOR_STATUS_DISABLED         0
#define SENSOR_STATUS_INITIALISED      1
#define SENSOR_STATUS_NOT_READY        2
#define SENSOR_STATUS_READY            3

#define TIME_BOOT                      5 //ticks

extern const struct sensors_sensor lis3mdl_mag_sensor;

#endif
