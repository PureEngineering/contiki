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
#ifndef lis2de12_accel_sensor_H_
#define lis2de12_accel_sensor_H_
/*---------------------------------------------------------------------------*/

#define ACCEL_X 1
#define ACCEL_Y 2
#define ACCEL_Z 3

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


#define TIME_BOOT                    5 // 5 ticks

extern const struct sensors_sensor lis2de12_accel_sensor;
/*---------------------------------------------------------------------------*/
#endif
