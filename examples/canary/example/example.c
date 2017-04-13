
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

//PROCESS_NAME(example);
//PROCESS(hey, "hey");
PROCESS(two, "two");
AUTOSTART_PROCESSES(&two);

static int light = 0;
static int bme1 = 0;
static int bme2 = 0;
static int bme3 = 0;
static int accel_x = 0;
static int accel_y = 0;
static int accel_z = 0;

static void getSensorReadings() {
    light = opt_3001_sensor.value(0);
    bme1 = bme_280_sensor.value(BME_280_SENSOR_TYPE_TEMP);
    bme2 = bme_280_sensor.value(BME_280_SENSOR_TYPE_PRESS);
    bme3 = bme_280_sensor.value(BME_280_SENSOR_TYPE_HUM);
    accel_x = lis2de12_accel_sensor.value(ACCEL_X);
    accel_y = lis2de12_accel_sensor.value(ACCEL_Y);
    accel_z = lis2de12_accel_sensor.value(ACCEL_Z);

    SENSORS_ACTIVATE(opt_3001_sensor);
    SENSORS_ACTIVATE(bme_280_sensor);
}

PROCESS_THREAD(two, ev, data) {
    PROCESS_BEGIN();
    static struct etimer et;
    pb_byte_t buffer[50];

    etimer_set(&et, CLOCK_SECOND / 2);

    SENSORS_ACTIVATE(lis2de12_accel_sensor);

    while (1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

        getSensorReadings();

        printf("LIGHT %i ", light);
        printf("BME1 %i ", bme1);
        printf("BME2 %i ", bme2);
        printf("BME3 %i ", bme3);
        printf("ACCEL_X %i ", accel_x);
        printf("ACCEL_Y %i ", accel_y);
        printf("ACCEL_Z %i ", accel_z);

        /*sensors message = sensors_init_zero;

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
        printf("\n");*/

        etimer_reset(&et);
    }
    PROCESS_END();
}