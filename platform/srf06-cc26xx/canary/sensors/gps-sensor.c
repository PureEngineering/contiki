#include "gps-sensor.h"
#include "lib/sensors.h"
#include "board-i2c.h"
#include "ti-lib.h"
#include "dev/cc26xx-uart.h"
#include "minmea.h"

static char line[MINMEA_MAX_LENGTH];
static int line_index = 0;

static int fix = 0;
static int lon = 0;
static int lat = 0;
static int height = 0;

static void parse_gga(const char *line) {
    switch (minmea_sentence_id(line, false)) {
        case MINMEA_SENTENCE_GGA: {
            struct minmea_sentence_gga frame;
            if (minmea_parse_gga(&frame, line)) {
                fix = frame.fix_quality;
                lon = (int) frame.longitude.value;
                lat = (int) frame.latitude.value;
                height = (int) frame.height.value;
            }
        }
    }
}

static int serial_line_input_byte(unsigned char ch) {
    line[line_index++] = ch;
    if(line[line_index - 2] == '\r' && line[line_index - 1] == '\n') {
        line[line_index++] = '\0';
        line_index = 0;
        parse_gga(line);
    }
    return 1;
}

static void init(void) {
    ti_lib_ioc_pin_type_gpio_output(IOID_16);
    ti_lib_gpio_write_dio(IOID_16, 1);

    ti_lib_ioc_pin_type_gpio_input(IOID_30);
    ti_lib_ioc_io_port_pull_set(IOID_30, IOC_IOPULL_DOWN);

    ti_lib_ioc_pin_type_gpio_output(IOID_10);
}

static void turnOn(void) {
    uint8_t val = 0;
    while(val == 0) {
        ti_lib_gpio_write_dio(IOID_10, 1);
        clock_delay_usec(100);
        ti_lib_gpio_write_dio(IOID_10, 0);
        clock_delay_usec(1000);
        val = (uint8_t) ti_lib_gpio_read_dio(IOID_30);
    }
    cc26xx_uart_set_input(serial_line_input_byte);
}

static void shutDown(void) {
    ti_lib_gpio_write_dio(IOID_16, 0);
    cc26xx_uart_set_input(NULL);
}

static int value(gps_field field) {
    int val = 0;
    switch(field) {
        case GPS_FIX:
            val = fix;
            break;
        case GPS_LON:
            val = lon;
            break;
        case GPS_LAT:
            val = lat;
            break;
        case GPS_H:
            val = height;
            break;
    }
    return val;
}

static int configure(int type, int enable){
    switch(type){
        case SENSORS_HW_INIT:
            init();
            break;
        case SENSORS_ACTIVE:
            if(enable) {
                turnOn();
            }else {
                shutDown();
            }
            break;
    }
    return 0;
}

static int status(int type) {
    return 0;
}

SENSORS_SENSOR(gps_sensor, "GPS", value, configure, status);