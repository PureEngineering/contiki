#include "gps-sensor.h"
#include "lib/sensors.h"
#include "board-i2c.h"
#include "ti-lib.h"

static void init(void) {
    //ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_GPS_RX);
    //ti_lib_ioc_port_configure_set(BOARD_IOID_GPS_RX, IOC_PORT_GPIO, IOC_IOPULL_UP);
    //ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_GPS_TX);
    //ti_lib_ioc_port_configure_set(BOARD_IOID_GPS_TX, IOC_PORT_GPIO, IOC_IOPULL_UP);

    ti_lib_ioc_pin_type_gpio_output(IOID_16);
    ti_lib_gpio_write_dio(IOID_16, 1);

    ti_lib_ioc_pin_type_gpio_input(IOID_30);
    ti_lib_ioc_io_port_pull_set(IOID_30, IOC_IOPULL_DOWN);

    ti_lib_ioc_pin_type_gpio_output(IOID_10);
    ti_lib_gpio_write_dio(IOID_10, 1);
    clock_delay_usec(100);
    ti_lib_gpio_write_dio(IOID_10, 0);
    clock_delay_usec(1000);

    int val = (int) ti_lib_gpio_read_dio(IOID_30);
    printf("yes %d", val);
}

static void read(void) {



}

static int value(int type) {
    return 0;
}

static int configure(int type, int enable){
    switch(type){
        case SENSORS_HW_INIT:
            init();
            break;
        case SENSORS_ACTIVE:
            if(enable) {

            }else {

            }
            break;
    }
    return 0;
}

static int status(int type) {
    return 0;
}

SENSORS_SENSOR(gps_sensor, "GPS", value, configure, status);