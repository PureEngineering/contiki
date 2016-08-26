/*
 * Copyright (c) 2012, STMicroelectronics.
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
 *
 * This file is part of the Contiki operating system.
 *
 */
/*---------------------------------------------------------------------------*/
#ifndef __LORA_RADIO_ARCH_CONFIG_H__
#define __LORA_RADIO_ARCH_CONFIG_H__
/*---------------------------------------------------------------------------*/
#define LORA_MAX_PAYLOAD_SIZE				255		// Max payload size in bytes
#define TX_OUTPUT_POWER					14		// dBm
#define TX_TIMEOUT_VALUE				3000000		// us

#define CCA_THRESHOLD					-120.0		// Clear-Channel Assessment Threshold (dBm)
#define RX_TIMEOUT_VALUE				0		// us (0 = Continuously listening)
#define RX_CONTINUOUS_MODE				(RX_TIMEOUT_VALUE ? false : true)

#if defined(USE_BAND_868)
  #define RF_FREQUENCY					868000000	// Hz
#elif defined(USE_BAND_915)
  #define RF_FREQUENCY					915000000	// Hz
#else
  #error "Please define a frequency band in the compiler options."
#endif

#if defined(USE_MODEM_LORA)
  #define LORA_BANDWIDTH				0		// [0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: Reserved]
  #define LORA_SPREADING_FACTOR				7		// [SF7..SF12]
  #define LORA_CODINGRATE				1		// [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
  #define LORA_PREAMBLE_LENGTH				8		// Same for Tx and Rx
  #define LORA_SYMBOL_TIMEOUT				5		// Symbols
  #define LORA_FIX_LENGTH_PAYLOAD_ON			false
  #define LORA_FIXED_PAYLOAD_LENGTH			0		// bytes
  #define LORA_CRC_ON					true
  #define LORA_FREQUENCY_HOPPING_ON			false
  #define LORA_HOPPING_PERIOD				0		// Symbols
  #define LORA_IQ_INVERSION_ON				false
#elif defined(USE_MODEM_FSK)
  #define FSK_FDEV					25e3		// Hz
  #define FSK_DATARATE					50e3		// bps
  #define FSK_BANDWIDTH					50e3		// Hz
  #define FSK_AFC_BANDWIDTH				83.333e3	// Hz
  #define FSK_PREAMBLE_LENGTH				5		// Same for Tx and Rx
  #define FSK_FIX_LENGTH_PAYLOAD_ON			false
  #define FSK_FIXED_PAYLOAD_LENGTH			0		// bytes
  #define FSK_CRC_ON					true
#else
  #error "Please define a modem in the compiler options."
#endif
/*---------------------------------------------------------------------------*/
#endif /* __LORA_RADIO_ARCH_CONFIG_H__ */
/*---------------------------------------------------------------------------*/
