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
#include "lora-radio-arch.h"
/*---------------------------------------------------------------------------*/
#define DEBUG 0
#if DEBUG
  #define PRINTF(...)	printf(__VA_ARGS__)
#else
  #define PRINTF(...)
#endif
/*---------------------------------------------------------------------------*/
#define CLEAR_RXBUF()           (lora_radio_rxbuf[0] = 0)
#define IS_RXBUF_EMPTY()        (lora_radio_rxbuf[0] == 0)
/*---------------------------------------------------------------------------*/
/* Incoming data buffer, the first byte will contain the length of the packet */
static uint8_t lora_radio_rxbuf[LORA_MAX_PAYLOAD_SIZE + 1];
static RadioEvents_t RadioEvents;
static int packet_is_prepared = 0;
static const void *packet_payload;
static unsigned short packet_payload_len = 0;
static packetbuf_attr_t last_rssi = 0;
/*---------------------------------------------------------------------------*/
static int lora_radio_init(void);
static int lora_radio_prepare(const void *payload, unsigned short payload_len);
static int lora_radio_transmit(unsigned short payload_len);
static int lora_radio_send(const void *data, unsigned short len);
static int lora_radio_read(void *buf, unsigned short bufsize);
static int lora_radio_channel_clear(void);
static int lora_radio_receiving_packet(void);
static int lora_radio_pending_packet(void);
static int lora_radio_on(void);
static int lora_radio_off(void);
void OnTxDone(void);
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);
void OnTxTimeout(void);
void OnRxTimeout(void);
void OnRxError(void);
/*---------------------------------------------------------------------------*/
PROCESS(lora_radio_process, "LoRa radio driver process");
/*---------------------------------------------------------------------------*/
const struct radio_driver lora_radio_driver =
{
  lora_radio_init,
  lora_radio_prepare,
  lora_radio_transmit,
  lora_radio_send,
  lora_radio_read,
  lora_radio_channel_clear,
  lora_radio_receiving_packet,
  lora_radio_pending_packet,
  lora_radio_on,
  lora_radio_off,
};
/*---------------------------------------------------------------------------*/
static int lora_radio_init(void)
{
  PRINTF("\nRADIO INIT IN\n");

  SpiInit(&SX1272.Spi, RADIO_MOSI, RADIO_MISO, RADIO_SCLK, NC);
  SX1272IoInit();

  /* Radio initialization */
  RadioEvents.TxDone = OnTxDone;
  RadioEvents.RxDone = OnRxDone;
  RadioEvents.TxTimeout = OnTxTimeout;
  RadioEvents.RxTimeout = OnRxTimeout;
  RadioEvents.RxError = OnRxError;

  Radio.Init(&RadioEvents);
  Radio.SetChannel(RF_FREQUENCY);

#if defined(USE_MODEM_LORA)
  Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
		LORA_SPREADING_FACTOR, LORA_CODINGRATE,
		LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON, LORA_CRC_ON,
		LORA_FREQUENCY_HOPPING_ON, LORA_HOPPING_PERIOD,
		LORA_IQ_INVERSION_ON, TX_TIMEOUT_VALUE);
  Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
		LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
		LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
		LORA_FIXED_PAYLOAD_LENGTH, LORA_CRC_ON,
		LORA_FREQUENCY_HOPPING_ON, LORA_HOPPING_PERIOD,
		LORA_IQ_INVERSION_ON, RX_CONTINUOUS_MODE);
#elif defined(USE_MODEM_FSK)
  Radio.SetTxConfig(MODEM_FSK, TX_OUTPUT_POWER, FSK_FDEV, 0,
		FSK_DATARATE, 0,
		FSK_PREAMBLE_LENGTH, FSK_FIX_LENGTH_PAYLOAD_ON,
		FSK_CRC_ON, 0, 0, 0, TX_TIMEOUT_VALUE);
  Radio.SetRxConfig(MODEM_FSK, FSK_BANDWIDTH, FSK_DATARATE,
		0, FSK_AFC_BANDWIDTH, FSK_PREAMBLE_LENGTH,
		0, FSK_FIX_LENGTH_PAYLOAD_ON, FSK_FIXED_PAYLOAD_LENGTH,
		FSK_CRC_ON, 0, 0, 0, RX_CONTINUOUS_MODE);
#else
  #error "Please define a modem in the compiler options."
#endif

  process_start(&lora_radio_process, NULL);

  PRINTF("RADIO INIT OUT\n");
  return 0;
}
/*---------------------------------------------------------------------------*/
static int lora_radio_prepare(const void *payload, unsigned short payload_len)
{
  PRINTF("PREPARE IN: %u bytes\n", payload_len);
  packet_is_prepared = 0;

  /* Checks if the payload length is supported */
  if(payload_len > LORA_MAX_PAYLOAD_SIZE) {
    return RADIO_TX_ERR;
  }

  packet_payload = payload;
  packet_payload_len = payload_len;
  packet_is_prepared = 1;

  PRINTF("PREPARE OUT\n");
  return RADIO_TX_OK;
}
/*---------------------------------------------------------------------------*/
static int lora_radio_transmit(unsigned short payload_len)
{
  PRINTF("TRANSMIT IN\n");

  if(!packet_is_prepared) {
    return RADIO_TX_ERR;
  }

  Radio.Send((uint8_t *)packet_payload, packet_payload_len);
  packet_is_prepared = 0;

  PRINTF("TRANSMIT OUT\n");
  return RADIO_TX_OK;
}
/*---------------------------------------------------------------------------*/
static int lora_radio_send(const void *payload, unsigned short payload_len)
{
  if(lora_radio_prepare(payload, payload_len) == RADIO_TX_ERR) {
    return RADIO_TX_ERR;
  }
  return lora_radio_transmit(payload_len);
}
/*---------------------------------------------------------------------------*/
static int lora_radio_read(void *buf, unsigned short bufsize)
{
  PRINTF("READ IN\n");

  /* Checks if the RX buffer is empty */
  if(IS_RXBUF_EMPTY()) {
    PRINTF("READ OUT: RX BUFFER EMPTY\n");
    return 0;
  }

  /* Checks if buffer has the correct size */
  if(bufsize < lora_radio_rxbuf[0]) {
    PRINTF("READ OUT: TOO SMALL BUFFER\n");
    return 0;
  }

  /* Copies the packet received */
  memcpy(buf, lora_radio_rxbuf+1, lora_radio_rxbuf[0]);
  packetbuf_set_attr(PACKETBUF_ATTR_RSSI, last_rssi);
  bufsize = lora_radio_rxbuf[0];
  CLEAR_RXBUF();

  PRINTF("READ OUT\n");
  return bufsize;
}
/*---------------------------------------------------------------------------*/
static int lora_radio_channel_clear(void)
{
  PRINTF("CHANNEL CLEAR IN\n");
  bool channel_clear;

#if defined(USE_MODEM_LORA)
  channel_clear = Radio.IsChannelFree(MODEM_LORA, RF_FREQUENCY, CCA_THRESHOLD);
#elif defined(USE_MODEM_FSK)
  channel_clear = Radio.IsChannelFree(MODEM_FSK, RF_FREQUENCY, CCA_THRESHOLD);
#else
  #error "Please define a modem in the compiler options."
#endif

  PRINTF("CHANNEL CLEAR OUT\n");
  return channel_clear;
}
/*---------------------------------------------------------------------------*/
static int lora_radio_receiving_packet(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static int lora_radio_pending_packet(void)
{
  PRINTF("PENDING PACKET\n");
  return !IS_RXBUF_EMPTY();
}
/*---------------------------------------------------------------------------*/
static int lora_radio_off(void)
{
  Radio.Sleep();
  PRINTF("RADIO OFF\n");
  return 0;
}
/*---------------------------------------------------------------------------*/
static int lora_radio_on(void)
{
  Radio.Rx(RX_TIMEOUT_VALUE);
  PRINTF("RADIO ON\n");
  return 0;
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(lora_radio_process, ev, data)
{
  PROCESS_BEGIN();

  PRINTF("LoRa radio: process started\n");
  int len;

  while(1) {
    PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);

    PRINTF("LoRa radio: polled\n");
    packetbuf_clear();
    len = lora_radio_read(packetbuf_dataptr(), PACKETBUF_SIZE);

    if(len > 0) {
      packetbuf_set_datalen(len);
      NETSTACK_RDC.input();
    }

    if(!IS_RXBUF_EMPTY()) {
      process_poll(&lora_radio_process);
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void OnTxDone(void)
{
  PRINTF("PACKET SENT\n");
  Radio.Rx(RX_TIMEOUT_VALUE);
}
/*---------------------------------------------------------------------------*/
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
  memcpy(lora_radio_rxbuf+1, payload, size);
  lora_radio_rxbuf[0] = size;
  PRINTF("PACKET RECEIVED\n");
  process_poll(&lora_radio_process);
  last_rssi = (packetbuf_attr_t)rssi;
}
/*---------------------------------------------------------------------------*/
void OnTxTimeout(void)
{
  PRINTF("TX TIMEOUT\n");
  Radio.Rx(RX_TIMEOUT_VALUE);
}
/*---------------------------------------------------------------------------*/
void OnRxTimeout(void)
{
  PRINTF("RX TIMEOUT\n");
  Radio.Rx(RX_TIMEOUT_VALUE);
}
/*---------------------------------------------------------------------------*/
void OnRxError(void)
{
  PRINTF("RX ERROR\n");
  Radio.Rx(RX_TIMEOUT_VALUE);
}
/*---------------------------------------------------------------------------*/
