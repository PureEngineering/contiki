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

#include "node-id.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//#include "lora-radio.h"
//#include "lora-sx1276-board.h"
char data_id[8] = "";
char *ids[8] = {"20357", "33794", "49541", "51072", "55428"};
enum {BATMON_TEMP, BATMON_VOLT, OPT_LIGHT, BME_TEMP, BME_PRES, BMEHUM, ACCEL_X, ACCEL_Y, ACCEL_Z, MAG_X, MAG_Y, MAG_Z};
char *sensor_names[16] = {"batmon_temp", "batmon_volt", "opt_3001", "bme_280_temp", "bme_280_pres", "bme_280_hum", "LIS2DE12_x", "LIS2DE12_y", "LIS2DE12_z", "lis3mdl_x", "lis3mdl_y", "lis3mdl_z"};
long sensor_data[100][256];
int lens[100];
/*---------------------------------------------------------------------------*/
PROCESS_NAME(cetic_6lbr_client_process);
PROCESS(cc26xx_web_demo_process, "CC26XX Web Demo");
process_event_t cc26xx_web_demo_publish_event;

#define SENSOR_READING_PERIOD (CLOCK_SECOND * 30)

struct ctimer batmon_timer, opt_timer, bme_timer, accel_timer, mag_timer;
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

static void init_light_reading(void *data);
static void init_bme_reading(void *data);
static void init_accel_reading(void *data);
static void init_mag_reading(void *data);

AUTOSTART_PROCESSES(&cc26xx_web_demo_process);

static void get_bme_reading() {
  clock_time_t next = SENSOR_READING_PERIOD;

  bme_temp_reading.raw = sensor_data[3][clock_time()%lens[3]];
  bme_pres_reading.raw = sensor_data[4][clock_time()%lens[4]];
  bme_hum_reading.raw = sensor_data[5][clock_time()%lens[5]];

  ctimer_set(&bme_timer, next, init_bme_reading, NULL);
}

static void get_light_reading() {
  clock_time_t next = SENSOR_READING_PERIOD;

  opt_reading.raw = sensor_data[2][clock_time()%lens[2]];

  ctimer_set(&opt_timer, next, init_light_reading, NULL);
}

static void get_accel_reading(){
  clock_time_t next = SENSOR_READING_PERIOD;

  accel_x_reading.raw = sensor_data[6][clock_time()%lens[6]];
  accel_y_reading.raw = sensor_data[7][clock_time()%lens[7]];
  accel_z_reading.raw = sensor_data[8][clock_time()%lens[8]];

  ctimer_set(&accel_timer, next, init_accel_reading, NULL);
}

static void get_mag_reading(){
  clock_time_t next = SENSOR_READING_PERIOD;

  mag_x_reading.raw = sensor_data[9][clock_time()%lens[9]];
  mag_y_reading.raw = sensor_data[10][clock_time()%lens[10]];
  mag_z_reading.raw = sensor_data[11][clock_time()%lens[11]];

  ctimer_set(&mag_timer, next, init_mag_reading, NULL);
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
  clock_time_t next = SENSOR_READING_PERIOD;

  batmon_temp_reading.raw = sensor_data[0][clock_time()%lens[0]];
  batmon_volt_reading.raw = sensor_data[1][clock_time()%lens[1]];

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

  return;
}
/*---------------------------------------------------------------------------*/
static void init_sensors(void) {

  
  strcpy(data_id, ids[node_id%5]);
  unsigned int i;
  FILE *data_file;
  char buff[255];
  for (i = 0; i < 12; i++) {
    char filename[300] = "../../../examples/canary/mqtt_protobuf_cooja/data/";
    strcat(filename, data_id);
    strcat(filename, "_");
    strcat(filename, sensor_names[i]);
    printf("%s\n", sensor_names[i]);

    data_file = fopen(filename, "r");
    if (data_file == NULL)
    {
        printf("oops");
        return 1;
    }
    unsigned int j = 0;
    while ((fgets(buff, 255, data_file) != NULL) && (j<100)) {
        sensor_data[i][j] = strtol(buff, NULL, 10);
        j++;
        //printf("%d\n", strtol(buff, NULL, 10));
    }
    lens[i] = j;
    fclose(data_file);
  }


  list_add(sensor_list, &batmon_temp_reading);
  list_add(sensor_list, &batmon_volt_reading);
  list_add(sensor_list, &opt_reading);
  list_add(sensor_list, &bme_temp_reading);
  list_add(sensor_list, &bme_pres_reading);
  list_add(sensor_list, &bme_hum_reading);
  list_add(sensor_list, &accel_x_reading);
  list_add(sensor_list, &accel_y_reading);
  list_add(sensor_list, &accel_z_reading);
  list_add(sensor_list, &mag_x_reading);
  list_add(sensor_list, &mag_y_reading);
  list_add(sensor_list, &mag_z_reading);
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
    PROCESS_YIELD();
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
