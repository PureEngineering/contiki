//
// Created by furqaan on 5/5/17.
//

#ifndef CONTIKI_MAG_SENSOR_H
#define CONTIKI_MAG_SENSOR_H

typedef enum {
    X, Y, Z
} mag_field;

extern const struct sensors_sensor mag_sensor;

#endif //CONTIKI_MAG_SENSOR_H
