
#include "contiki-net.h"
#include "contiki.h"
#include "batmon-sensor.h"
#include "board-peripherals.h"
#include "example.h"
#include "lib/list.h"
#include "lib/sensors.h"
#include "net/ipv6/sicslowpan.h"
#include "sys/process.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lora-radio.h"
#include "lora-sx1276-board.h"

//PROCESS_NAME(example);
PROCESS(hey, "hey");
PROCESS(two, "two");
AUTOSTART_PROCESSES(&two);

/*---------------------------------------------------------------------------*/
/*
 * Update sensor readings in a staggered fashion every SENSOR_READING_PERIOD
 * ticks + a random interval between 0 and SENSOR_READING_RANDOM ticks
 */
#define SENSOR_READING_PERIOD (CLOCK_SECOND * 25)

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


static void get_bme_reading() {
    int value;
    clock_time_t next = SENSOR_READING_PERIOD;

    value = bme_280_sensor.value(BME_280_SENSOR_TYPE_TEMP);
    if (value != CC26XX_SENSOR_READING_ERROR) {
        bme_temp_reading.raw = value;
    }

    value = bme_280_sensor.value(BME_280_SENSOR_TYPE_PRESS);
    if (value != CC26XX_SENSOR_READING_ERROR) {
        bme_pres_reading.raw = value;
    }

    value = bme_280_sensor.value(BME_280_SENSOR_TYPE_HUM);
    if (value != CC26XX_SENSOR_READING_ERROR) {
        bme_hum_reading.raw = value;
    }

    SENSORS_DEACTIVATE(bme_280_sensor);
    ctimer_set(&bme_timer, next, init_bme_reading, NULL);
}

static void get_light_reading() {
    int value;
    clock_time_t next = SENSOR_READING_PERIOD;

    value = opt_3001_sensor.value(0);

    if (value != CC26XX_SENSOR_READING_ERROR) {
        opt_reading.raw = value;
    }
    /* The OPT will turn itself off, so we don't need to call its DEACTIVATE */
    ctimer_set(&opt_timer, next, init_light_reading, NULL);
}

static void get_accel_reading(){
    int value;
    clock_time_t next = SENSOR_READING_PERIOD;

    value = lis2de12_accel_sensor.value(ACCEL_X); //read x value
    if(value!=CC26XX_SENSOR_READING_ERROR) {
        accel_x_reading.raw = value;
    }

    value = lis2de12_accel_sensor.value(ACCEL_Y); //read y value
    if(value!=CC26XX_SENSOR_READING_ERROR) {
        accel_y_reading.raw = value;
    }

    value = lis2de12_accel_sensor.value(ACCEL_Z); //read z value
    if(value!=CC26XX_SENSOR_READING_ERROR) {
        accel_z_reading.raw = value;
    }

    SENSORS_DEACTIVATE(lis2de12_accel_sensor);
    ctimer_set(&accel_timer, next, init_accel_reading, NULL);

}

static void get_mag_reading(){
    int value;
    clock_time_t next = SENSOR_READING_PERIOD;

    value = lis3mdl_mag_sensor.value(MAG_X); //read x value
    if(value!=CC26XX_SENSOR_READING_ERROR) {
        mag_x_reading.raw = value;
    }

    value = lis3mdl_mag_sensor.value(MAG_Y); //read y value
    if(value!=CC26XX_SENSOR_READING_ERROR) {
        mag_y_reading.raw = value;
    }

    value = lis3mdl_mag_sensor.value(MAG_Z); //read z value
    if(value!=CC26XX_SENSOR_READING_ERROR) {
        mag_z_reading.raw = value;
    }

    SENSORS_DEACTIVATE(lis3mdl_mag_sensor);
    ctimer_set(&mag_timer, next, init_mag_reading, NULL);
}

static void get_gas_reading(){
    int value;
    clock_time_t next = SENSOR_READING_PERIOD;

    value = gas_sensor.value(GAS_OX);
    if(value!=CC26XX_SENSOR_READING_ERROR) {
        gas_ox_reading.raw = value;
    }

    value = gas_sensor.value(GAS_NH3);
    if(value!=CC26XX_SENSOR_READING_ERROR) {
        gas_nh3_reading.raw = value;
    }

    value = gas_sensor.value(GAS_RED);
    if(value!=CC26XX_SENSOR_READING_ERROR) {
        gas_red_reading.raw = value;
    }

    SENSORS_DEACTIVATE(gas_sensor);
    ctimer_set(&gas_timer,next,init_gas_reading,NULL);
}

static void init_bme_reading(void *data) {
    SENSORS_ACTIVATE(bme_280_sensor);
}

static void init_light_reading(void *data) {
    SENSORS_ACTIVATE(opt_3001_sensor);
}

static void init_accel_reading(void *data) {
    SENSORS_ACTIVATE(lis2de12_accel_sensor);
}

static void init_mag_reading(void *data){
    SENSORS_ACTIVATE(lis3mdl_mag_sensor);
}

static void init_gas_reading(void *data){
    SENSORS_ACTIVATE(gas_sensor);
}
// sets up this process to be autostarted
/*---------------------------------------------------------------------------*/
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

    value = batmon_sensor.value(BATMON_SENSOR_TYPE_TEMP);
    if (value != CC26XX_SENSOR_READING_ERROR) {
        batmon_temp_reading.raw = value;
    }

    value = batmon_sensor.value(BATMON_SENSOR_TYPE_VOLT);
    if (value != CC26XX_SENSOR_READING_ERROR) {
        batmon_volt_reading.raw = value;
    }

    ctimer_set(&batmon_timer, next, get_batmon_reading, NULL);
}
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
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
    SENSORS_ACTIVATE(batmon_sensor);
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

static void getSensorReadings() {
    int value;

    SENSORS_ACTIVATE(opt_3001_sensor);
    //SENSORS_ACTIVATE(bme_280_sensor);

    value = opt_3001_sensor.value(0);

    if (value != CC26XX_SENSOR_READING_ERROR) {
        opt_reading.raw = value;
    }
}

PROCESS_THREAD(two, ev, data) {
    PROCESS_BEGIN();
    pb_byte_t buffer[50];

    while (1) {
        //getSensorReadings();
        SENSORS_ACTIVATE(opt_3001_sensor);
        //get_light_reading();

        //printf("two ");

        sensors message = sensors_init_zero;

        message.id = 100;

        message.has_batmon_temp = true;
        message.batmon_temp = 100;

        message.has_batmon_volt = true;
        message.batmon_volt = 100;

        message.has_opt_3001 = true;
        message.opt_3001 = 100;

        message.has_bme_280_temp = true;
        message.bme_280_temp = 100;

        message.has_bme_280_pres = true;
        message.bme_280_pres = 100;

        message.has_bme_280_hum = true;
        message.bme_280_hum = 100;

        message.has_LIS2DE12_x = true;
        message.LIS2DE12_x = 100;

        message.has_LIS2DE12_y = true;
        message.LIS2DE12_y = 100;

        message.has_LIS2DE12_z = true;
        message.LIS2DE12_z = 100;

        message.has_lis3mdl_x = true;
        message.lis3mdl_x = 100;

        message.has_lis3mdl_y = true;
        message.lis3mdl_y = 100;

        message.has_lis3mdl_z = true;
        message.lis3mdl_z = 100;

        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        bool status;
        status = pb_encode(&stream, sensors_fields, &message);
        size_t message_length;
        message_length = stream.bytes_written;

        int i;
        for (i = 0; i < message_length; i++) {
            printf("%c", buffer[i]);
        }
        printf("\n");

        PROCESS_YIELD();
    }
    PROCESS_END();
}

PROCESS_THREAD(hey, ev, data) {
    PROCESS_BEGIN();
    pb_byte_t buffer[70];
    char* buffer2[100];

    //process_start(&two, NULL);

    uint16_t unique_id = 100;

    init_sensors();

    init_sensor_readings();

    while (1) {
        //if (ev == sensors_event && data == &opt_3001_sensor) {
            get_light_reading();
        //}
        //if (ev == sensors_event && data == &bme_280_sensor) {
            get_bme_reading();
        //}
        //if (ev == sensors_event && data == &lis2de12_accel_sensor){
            get_accel_reading();
        //}
        //if (ev == sensors_event && data == &lis3mdl_mag_sensor){
            get_mag_reading();
        //}
        //if (ev ==sensors_event && data == &gas_sensor){
            get_gas_reading();
        //}

        printf("HI");

        /*sensors message = sensors_init_zero;

        //save values from sensors to message struct
        message.id = (int32_t)unique_id;

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

        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        bool status;
        status = pb_encode(&stream, sensors_fields, &message);
        size_t message_length;
        message_length = stream.bytes_written;

        //sprintf(buffer2, "%s\n", buffer);
        //printf(buffer2);

        //cc26xx_uart_write_byte('H');
        int i;
        for (i = 0; i < message_length; i++) {
            printf("%c", buffer[i]);
        }
        printf("\n");*/
        PROCESS_YIELD();
    }
    PROCESS_END();
}