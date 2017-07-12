#ifndef CONTIKI_GPS_SENSOR_H
#define CONTIKI_GPS_SENSOR_H


#define SENSOR_STATUS_DISABLED    0
#define SENSOR_STATUS_INITIALISED 1
#define SENSOR_STATUS_NOT_READY   2
#define SENSOR_STATUS_READY       3

#define GPS_FIX     0
#define GPS_LON     1
#define GPS_LAT     2
#define GPS_H       3

#define DEBUG_GPS   1
#if DEBUG_GPS
#define DBG_GPS(...) printf(__VA_ARGS__)
#else
#define DBG_GPS(...)
#endif

extern const struct sensors_sensor gps_sensor;

#endif
