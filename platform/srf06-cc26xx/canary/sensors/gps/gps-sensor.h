#ifndef CONTIKI_GPS_SENSOR_H
#define CONTIKI_GPS_SENSOR_H

extern const struct sensors_sensor gps_sensor;

typedef enum {
    GPS_FIX, GPS_LON, GPS_LAT, GPS_H
} gps_field;

#endif