//
// Created by furqaan on 5/5/17.
//

#ifndef CONTIKI_MAG_SENSOR_H
#define CONTIKI_MAG_SENSOR_H

extern const struct sensors_sensor mag_sensor;

typedef enum {
    M_X, M_Y, M_Z
} mag_field;

#endif //CONTIKI_MAG_SENSOR_H
