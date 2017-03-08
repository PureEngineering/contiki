#include "contiki-net.h"
#include "contiki.h"
//#include "batmon-sensor.h"
//#include "board-peripherals.h"
#include "cc26xx-web-demo.h"
#include "lib/list.h"
#include "lib/sensors.h"
#include "mqtt-client.h"
#include "net/ipv6/sicslowpan.h"
#include "sys/process.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "lora-radio.h"
//#include "lora-sx1276-board.h"
/*---------------------------------------------------------------------------*/
PROCESS_NAME(cetic_6lbr_client_process);
PROCESS(cc26xx_web_demo_process, "CC26XX Web Demo");
process_event_t cc26xx_web_demo_publish_event;

#define SENSOR_READING_PERIOD (CLOCK_SECOND * 30)

struct ctimer batmon_timer, opt_timer, bme_timer, accel_timer, mag_timer,
              gas_timer;

/*---------------------------------------------------------------------------*/
process_event_t cc26xx_web_demo_publish_event;

/* A cache of sensor values. Updated periodically*/
LIST(sensor_list);
/*---------------------------------------------------------------------------*/
/* The objects representing sensors used in this demo */
#define DEMO_SENSOR(name, type)                                                \
  cc26xx_web_demo_sensor_reading_t name##_reading = {NULL, 0, type}

// sensor structs to be linked later in linked list
DEMO_SENSOR(batmon_temp, CC26XX_WEB_DEMO_SENSOR_BATMON_TEMP);
DEMO_SENSOR(batmon_volt, CC26XX_WEB_DEMO_SENSOR_BATMON_VOLT);
DEMO_SENSOR(opt, CC26XX_WEB_DEMO_SENSOR_OPT_LIGHT);
DEMO_SENSOR(bme_pres, CC26XX_WEB_DEMO_SENSOR_BME_PRES);
DEMO_SENSOR(bme_temp, CC26XX_WEB_DEMO_SENSOR_BME_TEMP);
DEMO_SENSOR(bme_hum, CC26XX_WEB_DEMO_SENSOR_BME_HUM);
DEMO_SENSOR(accel_x, CC26XX_WEB_DEMO_SENSOR_ACCEL_X);
DEMO_SENSOR(accel_y, CC26XX_WEB_DEMO_SENSOR_ACCEL_Y);
DEMO_SENSOR(accel_z, CC26XX_WEB_DEMO_SENSOR_ACCEL_Z);
DEMO_SENSOR(mag_x, CC26XX_WEB_DEMO_SENSOR_MAG_X);
DEMO_SENSOR(mag_y, CC26XX_WEB_DEMO_SENSOR_MAG_Y);
DEMO_SENSOR(mag_z, CC26XX_WEB_DEMO_SENSOR_MAG_Z);
DEMO_SENSOR(gas_ox, CC26XX_WEB_DEMO_SENSOR_GAS_OX);
DEMO_SENSOR(gas_nh3, CC26XX_WEB_DEMO_SENSOR_GAS_NH3);
DEMO_SENSOR(gas_red, CC26XX_WEB_DEMO_SENSOR_GAS_RED);

static void init_light_reading(void *data);
static void init_bme_reading(void *data);
static void init_accel_reading(void *data);
static void init_mag_reading(void *data);
static void init_gas_reading(void *data);

AUTOSTART_PROCESSES(&cc26xx_web_demo_process);

static void get_bme_reading() {
  int value;
  clock_time_t next = SENSOR_READING_PERIOD;

  value = 10;
  //value = bme_280_sensor.value(BME_280_SENSOR_TYPE_TEMP);
  //if (value != CC26XX_SENSOR_READING_ERROR) {
    bme_temp_reading.raw = clock_time();
  //}

  value = 10;
  // value = bme_280_sensor.value(BME_280_SENSOR_TYPE_PRESS);
  //if (value != CC26XX_SENSOR_READING_ERROR) {
    bme_pres_reading.raw = clock_time();
 // }

  value = 10;
  // value = bme_280_sensor.value(BME_280_SENSOR_TYPE_HUM);
 // if (value != CC26XX_SENSOR_READING_ERROR) {
    bme_hum_reading.raw = value;
 // }

  //SENSORS_DEACTIVATE(bme_280_sensor);
  ctimer_set(&bme_timer, next, init_bme_reading, NULL);
}

static void get_light_reading() {
  int value;
  clock_time_t next = SENSOR_READING_PERIOD;

   value = 10;
  // value = opt_3001_sensor.value(0);

 // if (value != CC26XX_SENSOR_READING_ERROR) {
    opt_reading.raw = value;
 // }
  /* The OPT will turn itself off, so we don't need to call its DEACTIVATE */
  ctimer_set(&opt_timer, next, init_light_reading, NULL);
}

static void get_accel_reading(){
  int value;
  clock_time_t next = SENSOR_READING_PERIOD;

   value = 10;
  // value = lis2de12_accel_sensor.value(ACCEL_X); //read x value
 // if(value!=CC26XX_SENSOR_READING_ERROR) {
    accel_x_reading.raw = value;
  //}

   value = 10;
  // value = lis2de12_accel_sensor.value(ACCEL_Y); //read y value
  //if(value!=CC26XX_SENSOR_READING_ERROR) {
    accel_y_reading.raw = value;
  //}

   value = 10;
  // value = lis2de12_accel_sensor.value(ACCEL_Z); //read z value
  //if(value!=CC26XX_SENSOR_READING_ERROR) {
    accel_z_reading.raw = value;
 // }

  //SENSORS_DEACTIVATE(lis2de12_accel_sensor);
  ctimer_set(&accel_timer, next, init_accel_reading, NULL);

}

static void get_mag_reading(){
  int value;
  clock_time_t next = SENSOR_READING_PERIOD;

   value = 10;
  // value = lis3mdl_mag_sensor.value(MAG_X); //read x value
 // if(value!=CC26XX_SENSOR_READING_ERROR) {
    mag_x_reading.raw = value;
 // }

   value = 10;
  // value = lis3mdl_mag_sensor.value(MAG_Y); //read y value
 // if(value!=CC26XX_SENSOR_READING_ERROR) {
    mag_y_reading.raw = value;
 // }

   value = 10;
  // value = lis3mdl_mag_sensor.value(MAG_Z); //read z value
 // if(value!=CC26XX_SENSOR_READING_ERROR) {
    mag_z_reading.raw = value;
 // }

 // SENSORS_DEACTIVATE(lis3mdl_mag_sensor);
  ctimer_set(&mag_timer, next, init_mag_reading, NULL);
}

static void get_gas_reading(){
  int value;
  clock_time_t next = SENSOR_READING_PERIOD;

   value = 10;
  // value = gas_sensor.value(GAS_OX);
 // if(value!=CC26XX_SENSOR_READING_ERROR) {
    gas_ox_reading.raw = value;
 // }

   value = 10;
  // value = gas_sensor.value(GAS_NH3);
 // if(value!=CC26XX_SENSOR_READING_ERROR) {
    gas_nh3_reading.raw = value;
 // }

   value = 10;
  // value = gas_sensor.value(GAS_RED);
 // if(value!=CC26XX_SENSOR_READING_ERROR) {
    gas_red_reading.raw = value;
 // }

  //SENSORS_DEACTIVATE(gas_sensor);
  ctimer_set(&gas_timer,next,init_gas_reading,NULL);

}

static void init_bme_reading(void *data) {
  //SENSORS_ACTIVATE(bme_280_sensor);
}

static void init_light_reading(void *data) {
  //SENSORS_ACTIVATE(opt_3001_sensor);
}

static void init_accel_reading(void *data) {
 // SENSORS_ACTIVATE(lis2de12_accel_sensor);
}

static void init_mag_reading(void *data){
 // SENSORS_ACTIVATE(lis3mdl_mag_sensor);
}

static void init_gas_reading(void *data){
 // SENSORS_ACTIVATE(gas_sensor);
}
// sets up this process to be autostarted
int cc26xx_web_demo_ipaddr_sprintf(char *buf, uint8_t buf_len,
                                   const uip_ipaddr_t *addr) {
  uint16_t a;
  uint8_t len = 0;
  int i, f;
  for (i = 0, f = 0; i < sizeof(uip_ipaddr_t); i += 2) {
    a = (addr->u8[i] << 8) + addr->u8[i + 1];
    if (a == 0 && f >= 0) {
      if (f++ == 0) {
        len += snprintf(&buf[len], buf_len - len, "::");
      }
    } else {
      if (f > 0) {
        f = -1;
      } else if (i > 0) {
        len += snprintf(&buf[len], buf_len - len, ":");
      }
      len += snprintf(&buf[len], buf_len - len, "%x", a);
    }
  }

  return len;
}

/*---------------------------------------------------------------------------*/
const cc26xx_web_demo_sensor_reading_t *
cc26xx_web_demo_sensor_lookup(int sens_type) {
  cc26xx_web_demo_sensor_reading_t *reading = NULL;

  for (reading = list_head(sensor_list); reading != NULL;
       reading = list_item_next(reading)) {
    if (reading->type == sens_type) {
      return reading;
    }
  }

  return NULL;
}
/*---------------------------------------------------------------------------*/

const cc26xx_web_demo_sensor_reading_t *cc26xx_web_demo_sensor_first() {
  return list_head(sensor_list);
}

static void get_batmon_reading(void *data) {
  int value;
  clock_time_t next = SENSOR_READING_PERIOD;

   value = 10;
  // value = batmon_sensor.value(BATMON_SENSOR_TYPE_TEMP);
  //if (value != CC26XX_SENSOR_READING_ERROR) {
    batmon_temp_reading.raw = value;
 // }

   value = 10;
  // value = batmon_sensor.value(BATMON_SENSOR_TYPE_VOLT);
 // if (value != CC26XX_SENSOR_READING_ERROR) {
    batmon_volt_reading.raw = value;
  //}

  ctimer_set(&batmon_timer, next, get_batmon_reading, NULL);
}

static void init_sensor_readings(void) {
  /*
   * Make a first pass and get all initial sensor readings. This will also
   * trigger periodic value updates
   */
  get_batmon_reading(NULL);
  init_light_reading(NULL);
  init_bme_reading(NULL);
  init_accel_reading(NULL);
  init_mag_reading(NULL);
  init_gas_reading(NULL);

  return;
}
/*---------------------------------------------------------------------------*/
static void init_sensors(void) {

  list_add(sensor_list, &batmon_temp_reading);
  list_add(sensor_list, &batmon_volt_reading);
  list_add(sensor_list, &opt_reading);
  list_add(sensor_list, &bme_pres_reading);
  list_add(sensor_list, &bme_temp_reading);
  list_add(sensor_list, &bme_hum_reading);
  list_add(sensor_list, &accel_x_reading);
  list_add(sensor_list, &accel_y_reading);
  list_add(sensor_list, &accel_z_reading);
  list_add(sensor_list, &mag_x_reading);
  list_add(sensor_list, &mag_y_reading);
  list_add(sensor_list, &mag_z_reading);
  list_add(sensor_list, &gas_ox_reading);
  list_add(sensor_list, &gas_nh3_reading);
  list_add(sensor_list, &gas_red_reading);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(cc26xx_web_demo_process, ev, data) {
  PROCESS_BEGIN();

  printf("CC26XX Web Demo Process\n");

  init_sensors();

  cc26xx_web_demo_publish_event = process_alloc_event();

  process_start(&cetic_6lbr_client_process, NULL);
  process_start(&mqtt_client_process, NULL);

  init_sensor_readings();

  while (1) {

    process_post(PROCESS_BROADCAST, cc26xx_web_demo_publish_event, NULL);
    get_light_reading();
    get_bme_reading();
    get_accel_reading();
    get_mag_reading();
    get_gas_reading();
    PROCESS_YIELD();
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
