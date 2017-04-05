/*
 * Copyright (c) 2014, Texas Instruments Incorporated - http://www.ti.com/
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
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/**
 * \addtogroup cc26xx-examples
 * @{
 *
 * \defgroup cc26xx-web-demo CC26xx Web Demo
 * @{
 *
 *   An example demonstrating:
 *   * how to use a CC26XX-powered node in a deployment driven by a 6LBR
 *   * how to expose on-device sensors as CoAP resources
 *   * how to build a small web page which reports networking and sensory data
 *   * how to configure functionality through the aforementioned web page using
 *     HTTP POST requests
 *   * a network-based UART
 *
 * \file
 *   Main header file for the CC26XX web demo.
 */
/*---------------------------------------------------------------------------*/
#ifndef CC26XX_WEB_DEMO_H_
#define CC26XX_WEB_DEMO_H_
/*---------------------------------------------------------------------------*/
#include "dev/leds.h"
#include "sys/process.h"
#include "mqtt-client.h"
#include "net/ip/uip.h"
#include <stdint.h>

#include "pb_encode.h"
#include "pb_common.h"
#include "pb.h"
#include "sensor_message.pb.h"
#include "pb_decode.h"

#define CC26XX_WEB_DEMO_STATUS_LED LEDS_GREEN
/*---------------------------------------------------------------------------*/
/* A timeout used when waiting to connect to a network */
#define CC26XX_WEB_DEMO_NET_CONNECT_PERIODIC        (CLOCK_SECOND >> 3)
/*---------------------------------------------------------------------------*/
/* Default configuration values */
#define CC26XX_WEB_DEMO_DEFAULT_ORG_ID              "quickstart"

#define CC26XX_WEB_DEMO_DEFAULT_TYPE_ID             "cc26xx"
#define CC26XX_WEB_DEMO_DEFAULT_EVENT_TYPE_ID       "status"
#define CC26XX_WEB_DEMO_DEFAULT_SUBSCRIBE_CMD_TYPE  "+"
#define CC26XX_WEB_DEMO_DEFAULT_BROKER_PORT         1883
#define CC26XX_WEB_DEMO_DEFAULT_PUBLISH_INTERVAL    (CLOCK_SECOND)
#define CC26XX_WEB_DEMO_DEFAULT_KEEP_ALIVE_TIMER    60
#define CC26XX_WEB_DEMO_DEFAULT_RSSI_MEAS_INTERVAL  (CLOCK_SECOND)
/*---------------------------------------------------------------------------*/
/*
 * You normally won't have to change anything from here onwards unless you are
 * extending the example
 */
/*---------------------------------------------------------------------------*/
/* Sensor types */
#define CC26XX_WEB_DEMO_SENSOR_BATMON_TEMP   0
#define CC26XX_WEB_DEMO_SENSOR_BATMON_VOLT   1
#define CC26XX_WEB_DEMO_SENSOR_OPT_LIGHT     2
#define CC26XX_WEB_DEMO_SENSOR_BME_TEMP      3
#define CC26XX_WEB_DEMO_SENSOR_BME_PRES      4
#define CC26XX_WEB_DEMO_SENSOR_BME_HUM       5
#define CC26XX_WEB_DEMO_SENSOR_ACCEL_X       6
#define CC26XX_WEB_DEMO_SENSOR_ACCEL_Y       7
#define CC26XX_WEB_DEMO_SENSOR_ACCEL_Z       8
#define CC26XX_WEB_DEMO_SENSOR_MAG_X         9
#define CC26XX_WEB_DEMO_SENSOR_MAG_Y         10
#define CC26XX_WEB_DEMO_SENSOR_MAG_Z         11
#define CC26XX_WEB_DEMO_SENSOR_GAS_OX        12
#define CC26XX_WEB_DEMO_SENSOR_GAS_NH3       13
#define CC26XX_WEB_DEMO_SENSOR_GAS_RED       14

/*---------------------------------------------------------------------------*/
extern process_event_t cc26xx_web_demo_publish_event;
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* A data type for sensor readings, internally stored in a linked list */
#define CC26XX_WEB_DEMO_CONVERTED_LEN        12

typedef struct cc26xx_web_demo_sensor_reading {
  struct cc26xx_web_demo_sensor_reading *next;
  int raw;
  uint8_t type;

} cc26xx_web_demo_sensor_reading_t;
/*---------------------------------------------------------------------------*/

extern mqtt_client_config_t mqtt_config;
/*---------------------------------------------------------------------------*/
/**
 * \brief Performs a lookup for a reading of a specific type of sensor
 * \param sens_type CC26XX_WEB_DEMO_SENSOR_BATMON_TEMP...
 * \return A pointer to the reading data structure or NULL
 */
const cc26xx_web_demo_sensor_reading_t *cc26xx_web_demo_sensor_lookup(int sens_type);

/**
 * \brief Returns the first available sensor reading
 * \return A pointer to the reading data structure or NULL
 */
const cc26xx_web_demo_sensor_reading_t *cc26xx_web_demo_sensor_first(void);

/**
 * \brief Print an IPv6 address into a buffer
 * \param buf A pointer to the buffer where this function will print the IPv6
 *        address
 * \param buf_len the length of the buffer
 * \param addr A pointer to the IPv6 address
 * \return The number of bytes written to the buffer
 *
 * It is the caller's responsibility to allocate enough space for buf
 */
int cc26xx_web_demo_ipaddr_sprintf(char *buf, uint8_t buf_len, const uip_ipaddr_t *addr);

/**
 * \brief Resets the example to a default configuration
 */
void cc26xx_web_demo_restore_defaults(void);
/*---------------------------------------------------------------------------*/
#endif /* CC26XX_WEB_DEMO_H_ */
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 */
