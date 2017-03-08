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
 * \addtogroup cc26xx-web-demo
 * @{
 *
 * \file
 *   MQTT/IBM cloud service client for the CC26XX web demo.
 */
/*---------------------------------------------------------------------------*/
#include "mqtt-client.h"
//#include "board-peripherals.h"
#include "cc26xx-web-demo.h"
#include "contiki-conf.h"
#include "node-id.h"
#include "dev/leds.h"
#include "lib/sensors.h"
#include "mqtt.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-icmp6.h"
#include "net/rpl/rpl.h"
#include "rpl/rpl-private.h"
#include "sys/ctimer.h"
#include "sys/etimer.h"

#include <string.h>
#include <fcntl.h>
#include <strings.h>
/*---------------------------------------------------------------------------*/
/*
 * IBM server: messaging.quickstart.internetofthings.ibmcloud.com
 * (184.172.124.189) mapped in an NAT64 (prefix 64:ff9b::/96) IPv6 address
 * Note: If not able to connect; lookup the IP address again as it may change.
 *
 * If the node has a broker IP setting saved on flash, this value here will
 * get ignored
 */
static const char *broker_ip = "bbbb::101";

/*---------------------------------------------------------------------------*/
/*
 * A timeout used when waiting for something to happen (e.g. to connect or to
 * disconnect)
 */
#define STATE_MACHINE_PERIODIC (CLOCK_SECOND >> 1)
/*---------------------------------------------------------------------------*/
/* Provide visible feedback via LEDS during various states */
/* When connecting to broker */
#define CONNECTING_LED_DURATION (CLOCK_SECOND >> 3)

/* Each time we try to publish */
#define PUBLISH_LED_ON_DURATION (CLOCK_SECOND)
/*---------------------------------------------------------------------------*/
/* Connections and reconnections */
#define RETRY_FOREVER 0xFF
#define RECONNECT_INTERVAL (CLOCK_SECOND * 2)

/*
 * Number of times to try reconnecting to the broker.
 * Can be a limited number (e.g. 3, 10 etc) or can be set to RETRY_FOREVER
 */
#define RECONNECT_ATTEMPTS 5
#define CONNECTION_STABLE_TIME (CLOCK_SECOND * 5)
#define NEW_CONFIG_WAIT_INTERVAL (CLOCK_SECOND * 20)
static struct timer connection_life;
static uint8_t connect_attempt;
/*---------------------------------------------------------------------------*/
/* Various states */
static uint8_t state;
#define MQTT_CLIENT_STATE_INIT 0
#define MQTT_CLIENT_STATE_REGISTERED 1
#define MQTT_CLIENT_STATE_CONNECTING 2
#define MQTT_CLIENT_STATE_CONNECTED 3
#define MQTT_CLIENT_STATE_PUBLISHING 4
#define MQTT_CLIENT_STATE_DISCONNECTED 5
#define MQTT_CLIENT_STATE_NEWCONFIG 6
#define MQTT_CLIENT_STATE_CONFIG_ERROR 0xFE
#define MQTT_CLIENT_STATE_ERROR 0xFF
/*---------------------------------------------------------------------------*/
/* Maximum TCP segment size for outgoing segments of our socket */
#define MQTT_CLIENT_MAX_SEGMENT_SIZE 32
/*---------------------------------------------------------------------------*/
/*
 * Buffers for Client ID and Topic.
 * Make sure they are large enough to hold the entire respective string
 *
 * d:quickstart:status:EUI64 is 32 bytes long
 * iot-2/evt/status/fmt/json is 25 bytes
 * We also need space for the null termination
 */
#define BUFFER_SIZE 64
static char client_id[BUFFER_SIZE];
static char pub_topic[BUFFER_SIZE];
static char sub_topic[BUFFER_SIZE];

uint16_t unique_id =0;

//protobuf message gets encoded into this buffer in publish() function
uint8_t buffer[128];

static struct mqtt_connection conn;
/*---------------------------------------------------------------------------*/
#define QUICKSTART "quickstart"
/*---------------------------------------------------------------------------*/
static struct mqtt_message *msg_ptr = 0;
static struct etimer publish_periodic_timer;
static struct ctimer ct;
/*---------------------------------------------------------------------------*/
/* Parent RSSI functionality */
/*---------------------------------------------------------------------------*/
mqtt_client_config_t mqtt_config;
mqtt_client_config_t *conf = &mqtt_config;
/*---------------------------------------------------------------------------*/
PROCESS(mqtt_client_process, "CC26XX MQTT Client");
/*---------------------------------------------------------------------------*/
static void publish_led_off(void *d) { leds_off(CC26XX_WEB_DEMO_STATUS_LED); }
/*---------------------------------------------------------------------------*/

//will change code here once we implement subscribe commands
//might change whole send and recieve process flow at some point
static void pub_handler(const char *topic, uint16_t topic_len,
                        const uint8_t *chunk, uint16_t chunk_len) {
  DBG("Pub Handler: topic='%s' (len=%u), chunk_len=%u\n", topic, topic_len,
      chunk_len);

  /* If we don't like the length, ignore */
  if (topic_len != 23 || chunk_len != 1) {
    printf("Incorrect topic or chunk len. Ignored\n");
    return;
  }

  /* If the format != json, ignore */
  if (strncmp(&topic[topic_len - 4], "json", 4) != 0) {
    printf("Incorrect format\n");
  }

  if (strncmp(&topic[10], "leds", 4) == 0) {
    if (chunk[0] == '1') {
      leds_on(LEDS_RED);
    } else if (chunk[0] == '0') {
      leds_off(LEDS_RED);
    }
    return;
  }

#if BOARD_SENSORTAG
  if (strncmp(&topic[10], "buzz", 4) == 0) {
    if (chunk[0] == '1') {
      buzzer_start(1000);
    } else if (chunk[0] == '0') {
      buzzer_stop();
    }
    return;
  }
#endif
}
/*---------------------------------------------------------------------------*/
static void mqtt_event(struct mqtt_connection *m, mqtt_event_t event,
                       void *data) {
  switch (event) {
  case MQTT_EVENT_CONNECTED: {
    DBG("APP - Application has a MQTT connection\n");
    timer_set(&connection_life, CONNECTION_STABLE_TIME);
    state = MQTT_CLIENT_STATE_CONNECTED;
    break;
  }
  case MQTT_EVENT_DISCONNECTED: {
    DBG("APP - MQTT Disconnect. Reason %u\n", *((mqtt_event_t *)data));

    /* Do nothing if the disconnect was the result of an incoming config */
    if (state != MQTT_CLIENT_STATE_NEWCONFIG) {
      state = MQTT_CLIENT_STATE_DISCONNECTED;
      process_poll(&mqtt_client_process);
    }
    break;
  }
  case MQTT_EVENT_PUBLISH: {
    msg_ptr = data;

    /* Implement first_flag in publish message? */
    if (msg_ptr->first_chunk) {
      msg_ptr->first_chunk = 0;
      DBG("APP - Application received a publish on topic '%s'. Payload "
          "size is %i bytes. Content:\n\n",
          msg_ptr->topic, msg_ptr->payload_length);
    }

    pub_handler(msg_ptr->topic, strlen(msg_ptr->topic), msg_ptr->payload_chunk,
                msg_ptr->payload_length);
    break;
  }
  case MQTT_EVENT_SUBACK: {
    DBG("APP - Application is subscribed to topic successfully\n");
    break;
  }
  case MQTT_EVENT_UNSUBACK: {
    DBG("APP - Application is unsubscribed to topic successfully\n");
    break;
  }
  case MQTT_EVENT_PUBACK: {
    DBG("APP - Publishing complete.\n");
    break;
  }
  default:
    DBG("APP - Application got a unhandled MQTT event: %i\n", event);
    break;
  }
}
/*---------------------------------------------------------------------------*/
static int construct_pub_topic(void) {
  int len = snprintf(pub_topic, BUFFER_SIZE, "c"); //one char length topic

  /* len < 0: Error. Len >= BUFFER_SIZE: Buffer too small */
  if (len < 0 || len >= BUFFER_SIZE) {
    printf("Pub Topic: %d, Buffer %d\n", len, BUFFER_SIZE);
    return 0;
  }

  return 1;
}
/*---------------------------------------------------------------------------*/
static int construct_sub_topic(void) {
  int len =
      snprintf(sub_topic, BUFFER_SIZE, "iot-2/cmd/%s/fmt/json", conf->cmd_type);

  /* len < 0: Error. Len >= BUFFER_SIZE: Buffer too small */
  if (len < 0 || len >= BUFFER_SIZE) {
    printf("Sub Topic: %d, Buffer %d\n", len, BUFFER_SIZE);
    return 0;
  }

  return 1;
}
/*---------------------------------------------------------------------------*/
static int construct_client_id(void) {
/*
  uint16_t short_addr;
  uint8_t ext_addr[8];
  //ieee_addr_cpy_to(ext_addr, 8);
  short_addr = ext_addr[7];
  short_addr |= ext_addr[6] << 8;
  int myFile = open("/dev/random", O_RDONLY);            
  read(myFile, &short_addr, sizeof(short_addr)) ;
  close(myFile);
  unique_id = short_addr;
  */
  int len = snprintf(client_id, BUFFER_SIZE, "%02x", node_id);

  /* len < 0: Error. Len >= BUFFER_SIZE: Buffer too small */
  if (len < 0 || len >= BUFFER_SIZE) {
    printf("Client ID: %d, Buffer %d\n", len, BUFFER_SIZE);
    return 0;
  }

  return 1;
}

/*---------------------------------------------------------------------------*/
static void update_config(void) {
  if (construct_client_id() == 0) {
    /* Fatal error. Client ID larger than the buffer */
    state = MQTT_CLIENT_STATE_CONFIG_ERROR;
    return;
  }

  if (construct_sub_topic() == 0) {
    /* Fatal error. Topic larger than the buffer */
    state = MQTT_CLIENT_STATE_CONFIG_ERROR;
    return;
  }

  if (construct_pub_topic() == 0) {
    /* Fatal error. Topic larger than the buffer */
    state = MQTT_CLIENT_STATE_CONFIG_ERROR;
    return;
  }

  state = MQTT_CLIENT_STATE_INIT;

  /*
   * Schedule next timer event ASAP
   *
   * If we entered an error state then we won't do anything when it fires.
   *
   * Since the error at this stage is a config error, we will only exit this
   * error state if we get a new config.
   */
  etimer_set(&publish_periodic_timer, 0);

  return;
}
/*---------------------------------------------------------------------------*/
static int init_config() {
  /* Populate configuration with default values */
  memset(conf, 0, sizeof(mqtt_client_config_t));

  memcpy(conf->org_id, CC26XX_WEB_DEMO_DEFAULT_ORG_ID, 11);
  memcpy(conf->type_id, CC26XX_WEB_DEMO_DEFAULT_TYPE_ID, 7);
  memcpy(conf->event_type_id, CC26XX_WEB_DEMO_DEFAULT_EVENT_TYPE_ID, 7);
  memcpy(conf->broker_ip, broker_ip, strlen(broker_ip));
  memcpy(conf->cmd_type, CC26XX_WEB_DEMO_DEFAULT_SUBSCRIBE_CMD_TYPE, 1);

  conf->broker_port = CC26XX_WEB_DEMO_DEFAULT_BROKER_PORT;
  conf->pub_interval = CC26XX_WEB_DEMO_DEFAULT_PUBLISH_INTERVAL;

  return 1;
}

/*---------------------------------------------------------------------------*/
static void subscribe(void) {
  /* Publish MQTT topic in IBM quickstart format */
  mqtt_status_t status;

  status = mqtt_subscribe(&conn, NULL, sub_topic, MQTT_QOS_LEVEL_0);

  DBG("APP - Subscribing!\n");
  if (status == MQTT_STATUS_OUT_QUEUE_FULL) {
    DBG("APP - Tried to subscribe but command queue was full!\n");
  }
}
/*---------------------------------------------------------------------------*/
bool encode_string(pb_ostream_t *stream, const pb_field_t *field, void * const *arg)
{
    char *str = client_id;

    if (!pb_encode_tag_for_field(stream, field))
        return false;

    return pb_encode_string(stream, (uint8_t*)str, strlen(str));
}

bool print_string(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    uint8_t buffer[1024] = {0};

    /* We could read block-by-block to avoid the large buffer... */
    if (stream->bytes_left > sizeof(buffer) - 1)
        return false;

    if (!pb_read(stream, buffer, stream->bytes_left))
        return false;

    /* Print the string, in format comparable with protoc --decode.
     * Format comes from the arg defined in main().
     */
    printf((char*)*arg, buffer);
    return true;
}

static void publish(void) {

  int i=0;

  //initialize array with zeros
  memset(buffer, 0, sizeof(buffer));

  //initialize struct to default values (all false and zero)
  sensors message = sensors_init_zero;

  //save values from sensors to message struct
  message.id = node_id;


  message.has_batmon_temp = true;
  message.batmon_temp = cc26xx_web_demo_sensor_lookup(0)->raw;

  message.has_batmon_volt = true;
  message.batmon_volt = ((cc26xx_web_demo_sensor_lookup(1)->raw * 125)>>5);

  message.has_opt_3001 = true;
  message.opt_3001 = cc26xx_web_demo_sensor_lookup(2)->raw;

  message.has_bme_280_temp = true;
  message.bme_280_temp = cc26xx_web_demo_sensor_lookup(3)->raw;

  message.has_bme_280_pres = true;
  message.bme_280_pres = cc26xx_web_demo_sensor_lookup(4)->raw;

  message.has_bme_280_hum = true;
  message.bme_280_hum = cc26xx_web_demo_sensor_lookup(5)->raw;

  message.has_LIS2DE12_x = true;
  message.LIS2DE12_x = cc26xx_web_demo_sensor_lookup(6)->raw;

  message.has_LIS2DE12_y = true;
  message.LIS2DE12_y = cc26xx_web_demo_sensor_lookup(7)->raw;

  message.has_LIS2DE12_z = true;
  message.LIS2DE12_z = cc26xx_web_demo_sensor_lookup(8)->raw;

  message.has_lis3mdl_x = true;
  message.lis3mdl_x = cc26xx_web_demo_sensor_lookup(9)->raw;

  message.has_lis3mdl_y = true;
  message.lis3mdl_y = cc26xx_web_demo_sensor_lookup(10)->raw;

  message.has_lis3mdl_z = true;
  message.lis3mdl_z = cc26xx_web_demo_sensor_lookup(11)->raw;

  //stream used to encode to buffer
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

  //encode data from stream to buffer
  bool status;
  status = pb_encode(&stream, sensors_fields, &message);

  //how many bytes were written
  size_t message_length;
  message_length = stream.bytes_written;

  if (!status) {
    printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
  }

  //print encoded message, should be around 20-30 bytes
  DBG("Encoded Message %d bytes: ",message_length);
  for (i = 0; i < message_length; i++) {
    DBG("%x ", buffer[i]);
  }
  DBG("\n");

  //push encoded message to mqtt broker
  mqtt_publish(&conn, NULL, pub_topic, buffer, message_length, MQTT_QOS_LEVEL_0,
               MQTT_RETAIN_OFF);

  //all of decoding message (error handling)
  // sensors inmessage = sensors_init_zero;
  // pb_istream_t instream = pb_istream_from_buffer(buffer, message_length);
  //
  // status = pb_decode(&instream, sensors_fields, &inmessage);
  // if (!status) {
  //   printf("failed decode\n");
  // } else {
  //   printf("\ndecoded...\n");
  //   printf("batmon_temp %ld\n", inmessage.batmon_temp);
  //   printf("batmon_volt %ld\n", inmessage.batmon_volt);
  //   printf("opt_3001 %ld\n", inmessage.opt_3001);
  //   printf("bme_280_temp %ld\n", inmessage.bme_280_temp);
  //   printf("bme_280_pres %ld\n", inmessage.bme_280_pres);
  //   printf("bme_280_hum %ld\n", inmessage.bme_280_hum);
  // }

  DBG("APP - Publish!\n");
}
/*---------------------------------------------------------------------------*/
static void connect_to_broker(void) {
  /* Connect to MQTT server */
  mqtt_connect(&conn, conf->broker_ip, conf->broker_port,
               conf->pub_interval * 3);

  state = MQTT_CLIENT_STATE_CONNECTING;
}
/*---------------------------------------------------------------------------*/
static void state_machine(void) {
  switch (state) {
  case MQTT_CLIENT_STATE_INIT:
    /* If we have just been configured register MQTT connection */
    mqtt_register(&conn, &mqtt_client_process, client_id, mqtt_event,
                  MQTT_CLIENT_MAX_SEGMENT_SIZE);

    /*
     * If we are not using the quickstart service (thus we are an IBM
     * registered device), we need to provide user name and password
     */
    if (strncasecmp(conf->org_id, QUICKSTART, strlen(conf->org_id)) != 0) {
      if (strlen(conf->auth_token) == 0) {
        printf("User name set, but empty auth token\n");
        state = MQTT_CLIENT_STATE_ERROR;
        break;
      } else {
        mqtt_set_username_password(&conn, "use-token-auth", conf->auth_token);
      }
    }

    /* _register() will set auto_reconnect. We don't want that. */
    conn.auto_reconnect = 0;
    connect_attempt = 1;

    state = MQTT_CLIENT_STATE_REGISTERED;
  /* Continue */
  case MQTT_CLIENT_STATE_REGISTERED:
    if (uip_ds6_get_global(ADDR_PREFERRED) != NULL) {
      /* Registered and with a public IP. Connect */
      DBG("Registered. Connect attempt %u\n", connect_attempt);
      connect_to_broker();
    }
    etimer_set(&publish_periodic_timer, CC26XX_WEB_DEMO_NET_CONNECT_PERIODIC);
    return;
    break;
  case MQTT_CLIENT_STATE_CONNECTING:
    leds_on(CC26XX_WEB_DEMO_STATUS_LED);
    ctimer_set(&ct, CONNECTING_LED_DURATION, publish_led_off, NULL);
    /* Not connected yet. Wait */
    DBG("Connecting (%u)\n", connect_attempt);
    break;
  case MQTT_CLIENT_STATE_CONNECTED:
    /* Don't subscribe unless we are a registered device */
    if (strncasecmp(conf->org_id, QUICKSTART, strlen(conf->org_id)) == 0) {
      DBG("Using 'quickstart': Skipping subscribe\n");
      state = MQTT_CLIENT_STATE_PUBLISHING;
    }
  /* Continue */
  case MQTT_CLIENT_STATE_PUBLISHING:
    /* If the timer expired, the connection is stable. */
    if (timer_expired(&connection_life)) {
      /*
       * Intentionally using 0 here instead of 1: We want RECONNECT_ATTEMPTS
       * attempts if we disconnect after a successful connect
       */
      connect_attempt = 0;
    }

    if (mqtt_ready(&conn) && conn.out_buffer_sent) {
      /* Connected. Publish */
      if (state == MQTT_CLIENT_STATE_CONNECTED) {
        subscribe();
        state = MQTT_CLIENT_STATE_PUBLISHING;
      } else {
        leds_on(CC26XX_WEB_DEMO_STATUS_LED);
        ctimer_set(&ct, PUBLISH_LED_ON_DURATION, publish_led_off, NULL);
        publish();
      }
      etimer_set(&publish_periodic_timer, conf->pub_interval);
      /* Return here so we don't end up rescheduling the timer */
      return;
    } else {
      /*
       * Our publish timer fired, but some MQTT packet is already in flight
       * (either not sent at all, or sent but not fully ACKd).
       *
       * This can mean that we have lost connectivity to our broker or that
       * simply there is some network delay. In both cases, we refuse to
       * trigger a new message and we wait for TCP to either ACK the entire
       * packet after retries, or to timeout and notify us.
       */
      DBG("Publishing... (MQTT state=%d, q=%u)\n", conn.state,
          conn.out_queue_full);
    }
    break;
  case MQTT_CLIENT_STATE_DISCONNECTED:
    DBG("Disconnected\n");
    if (connect_attempt < RECONNECT_ATTEMPTS ||
        RECONNECT_ATTEMPTS == RETRY_FOREVER) {
      /* Disconnect and backoff */
      clock_time_t interval;
      mqtt_disconnect(&conn);
      connect_attempt++;

      interval = connect_attempt < 3 ? RECONNECT_INTERVAL << connect_attempt
                                     : RECONNECT_INTERVAL << 3;

      DBG("Disconnected. Attempt %u in %lu ticks\n", connect_attempt, interval);

      etimer_set(&publish_periodic_timer, interval);

      state = MQTT_CLIENT_STATE_REGISTERED;
      return;
    } else {
      /* Max reconnect attempts reached. Enter error state */
      state = MQTT_CLIENT_STATE_ERROR;
      DBG("Aborting connection after %u attempts\n", connect_attempt - 1);
    }
    break;
  case MQTT_CLIENT_STATE_NEWCONFIG:
    /* Only update config after we have disconnected */
    if (conn.state == MQTT_CONN_STATE_NOT_CONNECTED) {
      update_config();
      DBG("New config\n");

      /* update_config() scheduled next pass. Return */
      return;
    }
    break;
  case MQTT_CLIENT_STATE_CONFIG_ERROR:
    /* Idle away. The only way out is a new config */
    printf("Bad configuration.\n");
    return;
  case MQTT_CLIENT_STATE_ERROR:
  default:
    leds_on(CC26XX_WEB_DEMO_STATUS_LED);
    /*
     * 'default' should never happen.
     *
     * If we enter here it's because of some error. Stop timers. The only thing
     * that can bring us out is a new config event
     */
    printf("Default case: State=0x%02x\n", state);
    return;
  }

  /* If we didn't return so far, reschedule ourselves */
  etimer_set(&publish_periodic_timer, STATE_MACHINE_PERIODIC);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(mqtt_client_process, ev, data) {

  PROCESS_BEGIN();

  printf("CC26XX MQTT Client Process\n");

  //conf = &cc26xx_web_demo_config.mqtt_config;
  if (init_config() != 1) {
    PROCESS_EXIT();
  }

  update_config();

  /* Main loop */
  while (1) {

    PROCESS_YIELD();

    if ((ev == PROCESS_EVENT_TIMER && data == &publish_periodic_timer) ||
        ev == PROCESS_EVENT_POLL || ev == cc26xx_web_demo_publish_event) {

      state_machine();
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 */
