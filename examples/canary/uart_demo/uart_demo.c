
#include "contiki-net.h"
#include "contiki.h"
#include "batmon-sensor.h"
#include "board-peripherals.h"
#include "uart_demo.h"
#include "lib/list.h"
#include "lib/sensors.h"
#include "net/ipv6/sicslowpan.h"
#include "sys/process.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//PROCESS_NAME(uart_demo);
PROCESS(uart_demo, "uart_demo");
AUTOSTART_PROCESSES(&uart_demo);

static int light = 0;
static int bme1 = 0;
static int bme2 = 0;
static int bme3 = 0;
static int accel_x = 0;
static int accel_y = 0;
static int accel_z = 0;
static int mag_x = 0;
static int mag_y = 0;
static int mag_z = 0;
static int gas_ox = 0;
static int gas_nh3 = 0;
static int gas_red = 0;

static void getSensorReadings() {
    light = opt_3001_sensor.value(0);
    bme1 = bme_280_sensor.value(BME_280_SENSOR_TYPE_TEMP);
    bme2 = bme_280_sensor.value(BME_280_SENSOR_TYPE_PRESS);
    bme3 = bme_280_sensor.value(BME_280_SENSOR_TYPE_HUM);
    accel_x = lis2de12_accel_sensor.value(ACCEL_X);
    accel_y = lis2de12_accel_sensor.value(ACCEL_Y);
    accel_z = lis2de12_accel_sensor.value(ACCEL_Z);
    mag_x = lis3mdl_mag_sensor.value(MAG_X);
    mag_y = lis3mdl_mag_sensor.value(MAG_Y);
    mag_z = lis3mdl_mag_sensor.value(MAG_Z);
    gas_ox = gas_sensor.value(GAS_OX);
    gas_nh3 = gas_sensor.value(GAS_NH3);
    gas_red = gas_sensor.value(GAS_RED);

    SENSORS_ACTIVATE(opt_3001_sensor);
    SENSORS_ACTIVATE(bme_280_sensor);
    SENSORS_ACTIVATE(lis2de12_accel_sensor);
    SENSORS_ACTIVATE(lis3mdl_mag_sensor);
    SENSORS_ACTIVATE(gas_sensor);
}

PROCESS_THREAD(uart_demo, ev, data) {
    PROCESS_BEGIN();
    static struct etimer et;
    pb_byte_t buffer[50];
    char str[60];

    //cc26xx_uart_init();

    etimer_set(&et, CLOCK_SECOND / 25);

    while (1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

        getSensorReadings();

        sensors message = sensors_init_zero;

        int dummy = 1000;

        message.id = dummy;

        message.has_batmon_temp = true;
        message.batmon_temp = dummy;

        message.has_batmon_volt = true;
        message.batmon_volt = dummy;

        message.has_opt_3001 = true;
        message.opt_3001 = light;

        message.has_bme_280_temp = true;
        message.bme_280_temp = bme1;

        message.has_bme_280_pres = true;
        message.bme_280_pres = bme2;

        message.has_bme_280_hum = true;
        message.bme_280_hum = bme3;

        message.has_LIS2DE12_x = true;
        message.LIS2DE12_x = accel_x;

        message.has_LIS2DE12_y = true;
        message.LIS2DE12_y = accel_y;

        message.has_LIS2DE12_z = true;
        message.LIS2DE12_z = accel_z;

        message.has_lis3mdl_x = true;
        message.lis3mdl_x = mag_x;

        message.has_lis3mdl_y = true;
        message.lis3mdl_y = mag_y;

        message.has_lis3mdl_z = true;
        message.lis3mdl_z = mag_z;

        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
        bool status;
        status = pb_encode(&stream, sensors_fields, &message);
        size_t message_length;
        message_length = stream.bytes_written;

        //sprintf(str, "%s\n", buffer);
        printf("%d", (int) message_length);

        int i;
        i = 0;
        while(1) {

            if(i < message_length)
                if(cc26xx_uart_busy() == 0)
                    cc26xx_uart_write_byte(buffer[i++]);

            if(i == message_length) {
                if(cc26xx_uart_busy() == 0) {
                    cc26xx_uart_write_byte('\n');
                    i++;
                }
            }

            if(i == message_length + 1)
                break;
        }

        etimer_reset(&et);
    }
    PROCESS_END();
}