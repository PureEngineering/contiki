#include "gps-sensor.h"
#include "lib/sensors.h"
#include "ti-lib.h"
#include "dev/cc26xx-uart.h"
#include "minmea.h"
#include "sys/ctimer.h"


static char line[MINMEA_MAX_LENGTH];
static int line_index = 0;

static int fix = 0;
static int lon = 0;
static int lat = 0;
static int height = 0;

/* status variable */
static int enabled = SENSOR_STATUS_DISABLED;

/* timer used to wait for lockon */
static struct ctimer nofix_timer, startup_timer;

#define NOFIX_TIME      60 * 15 * CLOCK_SECOND
#define STARTUP_TIME    1 * CLOCK_SECOND

static void shutDown(void* not_used);
static void turnOn(void* not_used);
/*
* \brief Broadcast sensor_changed event for gps sensor
* \param not_used: non used variable, needed to be void pointer for ctimer
* \return none
*/
static void notify_ready(void *not_used){
  enabled = SENSOR_STATUS_READY;
  sensors_changed(&gps_sensor);
}

static void parse_gga(const char *line) {
    switch (minmea_sentence_id(line, false)) {
        case MINMEA_SENTENCE_GGA: {
            struct minmea_sentence_gga frame;
            if (minmea_parse_gga(&frame, line)) {
                if(fix > 0){
                  DBG_GPS("fix >0\n");
                  fix = frame.fix_quality;
                  lon = (int) frame.longitude.value;
                  lat = (int) frame.latitude.value;
                  height = (int) frame.height.value;
                  printf("fix %d\nlon %d\nlat %d\n height %d\n",fix,lon,lat,height);
                  ctimer_stop(&nofix_timer);
                  shutDown(NULL);
                  notify_ready(NULL);
                }
            }
        }
    }
}

static int serial_line_input_byte(unsigned char ch) {
    if(line_index == MINMEA_MAX_LENGTH){
      DBG_GPS("GPS-SESNOR -- Line reached end of array\n");
      DBG_GPS("%s\n",line);
      line_index =0;
    }
    line[line_index++] = ch;
    if(line[line_index - 2] == '\r' && line[line_index - 1] == '\n') {
        line[line_index++] = '\0';
        line_index = 0;
        parse_gga(line);
        DBG_GPS("%s\n",line);
    }
    return 1;
}

static void init(void) {
    DBG_GPS("gps init\n");
    ti_lib_ioc_pin_type_gpio_output(BOARD_IOID_GPS_ENABLE);
    ti_lib_gpio_write_dio(BOARD_IOID_GPS_ENABLE, 1);

    ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_GPS_WAKEUP);
    ti_lib_ioc_io_port_pull_set(BOARD_IOID_GPS_WAKEUP, IOC_IOPULL_DOWN);

    ti_lib_ioc_pin_type_gpio_output(BOARD_IOID_GPS_ON_OFF);

    enabled = SENSOR_STATUS_INITIALISED;
}

static void turnOn(void *not_used) {
    uint8_t val = 0;
    while(1) {
        ti_lib_gpio_write_dio(BOARD_IOID_GPS_ON_OFF, 1);
        clock_delay_usec(100);
        ti_lib_gpio_write_dio(BOARD_IOID_GPS_ON_OFF, 0);
        clock_delay_usec(1000);
        val = (uint8_t) ti_lib_gpio_read_dio(BOARD_IOID_GPS_WAKEUP);
        if(val == 1)
          break;
        ctimer_set(&startup_timer,STARTUP_TIME,turnOn,NULL);
    }
    enabled = SENSOR_STATUS_NOT_READY;
    cc26xx_uart_set_input(serial_line_input_byte);
    //ctimer_set(&nofix_timer,NOFIX_TIME,shutDown,NULL);
}

static void shutDown(void *not_used){
    #if DEBUG_GPS
    if(fix==0){
      DBG_GPS("NO FIX\n");
    }
    DBG_GPS("SHUTDOWN\n");
    #endif
    enabled = SENSOR_STATUS_DISABLED;
    ti_lib_gpio_write_dio(BOARD_IOID_GPS_ENABLE, 0);
    cc26xx_uart_set_input(NULL);
}

static int value(int field) {
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
                turnOn(NULL);
            }else {
                shutDown(NULL);
            }
            break;
    }
    return 0;
}

static int status(int type) {
    return enabled;
}

SENSORS_SENSOR(gps_sensor, "GPS", value, configure, status);
