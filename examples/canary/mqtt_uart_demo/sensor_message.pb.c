/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.3.6 at Tue Jul 19 10:17:41 2016. */

#include "sensor_message.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif



const pb_field_t sensors_fields[15] = {
    PB_FIELD(  1, INT32   , REQUIRED, STATIC  , FIRST, sensors, id, id, 0),
    PB_FIELD(  2, INT32   , OPTIONAL, STATIC  , OTHER, sensors, batmon_temp, id, 0),
    PB_FIELD(  3, INT32   , OPTIONAL, STATIC  , OTHER, sensors, batmon_volt, batmon_temp, 0),
    PB_FIELD(  4, INT32   , OPTIONAL, STATIC  , OTHER, sensors, opt_3001, batmon_volt, 0),
    PB_FIELD(  5, SINT32  , OPTIONAL, STATIC  , OTHER, sensors, bme_280_temp, opt_3001, 0),
    PB_FIELD(  6, INT32   , OPTIONAL, STATIC  , OTHER, sensors, bme_280_pres, bme_280_temp, 0),
    PB_FIELD(  7, INT32   , OPTIONAL, STATIC  , OTHER, sensors, bme_280_hum, bme_280_pres, 0),
    PB_FIELD(  8, SINT32  , OPTIONAL, STATIC  , OTHER, sensors, LIS2DE12_x, bme_280_hum, 0),
    PB_FIELD(  9, SINT32  , OPTIONAL, STATIC  , OTHER, sensors, LIS2DE12_y, LIS2DE12_x, 0),
    PB_FIELD( 10, SINT32  , OPTIONAL, STATIC  , OTHER, sensors, LIS2DE12_z, LIS2DE12_y, 0),
    PB_FIELD( 11, SINT32  , OPTIONAL, STATIC  , OTHER, sensors, lis3mdl_x, LIS2DE12_z, 0),
    PB_FIELD( 12, SINT32  , OPTIONAL, STATIC  , OTHER, sensors, lis3mdl_y, lis3mdl_x, 0),
    PB_FIELD( 13, SINT32  , OPTIONAL, STATIC  , OTHER, sensors, lis3mdl_z, lis3mdl_y, 0),
    PB_FIELD( 14, BOOL    , OPTIONAL, STATIC  , OTHER, sensors, pir, lis3mdl_z, 0),
    PB_LAST_FIELD
};


/* @@protoc_insertion_point(eof) */