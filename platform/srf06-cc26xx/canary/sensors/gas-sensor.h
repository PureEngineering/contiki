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
