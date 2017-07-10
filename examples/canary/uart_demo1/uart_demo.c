/**
  * \file
  *         A very simple Contiki application showing how Contiki
programs look
  * \author
  *         Adam Dunkels <adam@...>
  */

#include "contiki.h"
#include "dev/serial-line.h"
#include "dev/cc26xx-uart.h"

#include <stdio.h> /* For printf() */

#include "contiki-net.h"
#include "batmon-sensor.h"
#include "board-peripherals.h"
#include "uart_demo.h"
#include "lib/list.h"
#include "lib/sensors.h"
#include "net/ipv6/sicslowpan.h"
#include "sys/process.h"
#include "minmea.h"

static char line[MINMEA_MAX_LENGTH];
static int line_index = 0;

#define INDENT_SPACES "  "

void parse_gga(const char *line) {
    switch (minmea_sentence_id(line, false)) {
        case MINMEA_SENTENCE_GGA: {
            struct minmea_sentence_gga frame;
            if (minmea_parse_gga(&frame, line)) {
                printf(INDENT_SPACES "$xxGGA: fix quality: %d long %d lat %d height %d\n", frame.fix_quality, frame.longitude, frame.height);
                int f = (int) frame.longitude.value;
            }
            else {
                printf(INDENT_SPACES "$xxGGA sentence is not parsed\n");
            }
        }
    }
}

void parse_line(const char *line) {
    switch (minmea_sentence_id(line, false)) {
        case MINMEA_SENTENCE_RMC: {
            struct minmea_sentence_rmc frame;
            if (minmea_parse_rmc(&frame, line)) {
                printf(INDENT_SPACES "$xxRMC: raw coordinates and speed: (%d/%d,%d/%d) %d/%d\n",
                       frame.latitude.value, frame.latitude.scale,
                       frame.longitude.value, frame.longitude.scale,
                       frame.speed.value, frame.speed.scale);
                printf(INDENT_SPACES "$xxRMC fixed-point coordinates and speed scaled to three decimal places: (%d,%d) %d\n",
                       minmea_rescale(&frame.latitude, 1000),
                       minmea_rescale(&frame.longitude, 1000),
                       minmea_rescale(&frame.speed, 1000));
                printf(INDENT_SPACES "$xxRMC floating point degree coordinates and speed: (%f,%f) %f\n",
                       minmea_tocoord(&frame.latitude),
                       minmea_tocoord(&frame.longitude),
                       minmea_tofloat(&frame.speed));
            }
            else {
                printf(INDENT_SPACES "$xxRMC sentence is not parsed\n");
            }
        } break;

        case MINMEA_SENTENCE_GGA: {
            struct minmea_sentence_gga frame;
            if (minmea_parse_gga(&frame, line)) {
                printf(INDENT_SPACES "$xxGGA: fix quality: %d\n", frame.fix_quality);
            }
            else {
                printf(INDENT_SPACES "$xxGGA sentence is not parsed\n");
            }
        } break;

        case MINMEA_SENTENCE_GST: {
            struct minmea_sentence_gst frame;
            if (minmea_parse_gst(&frame, line)) {
                printf(INDENT_SPACES "$xxGST: raw latitude,longitude and altitude error deviation: (%d/%d,%d/%d,%d/%d)\n",
                       frame.latitude_error_deviation.value, frame.latitude_error_deviation.scale,
                       frame.longitude_error_deviation.value, frame.longitude_error_deviation.scale,
                       frame.altitude_error_deviation.value, frame.altitude_error_deviation.scale);
                printf(INDENT_SPACES "$xxGST fixed point latitude,longitude and altitude error deviation"
                               " scaled to one decimal place: (%d,%d,%d)\n",
                       minmea_rescale(&frame.latitude_error_deviation, 10),
                       minmea_rescale(&frame.longitude_error_deviation, 10),
                       minmea_rescale(&frame.altitude_error_deviation, 10));
                printf(INDENT_SPACES "$xxGST floating point degree latitude, longitude and altitude error deviation: (%f,%f,%f)",
                       minmea_tofloat(&frame.latitude_error_deviation),
                       minmea_tofloat(&frame.longitude_error_deviation),
                       minmea_tofloat(&frame.altitude_error_deviation));
            }
            else {
                printf(INDENT_SPACES "$xxGST sentence is not parsed\n");
            }
        } break;

        case MINMEA_SENTENCE_GSV: {
            struct minmea_sentence_gsv frame;
            if (minmea_parse_gsv(&frame, line)) {
                printf(INDENT_SPACES "$xxGSV: message %d of %d\n", frame.msg_nr, frame.total_msgs);
                printf(INDENT_SPACES "$xxGSV: sattelites in view: %d\n", frame.total_sats);
                for (int i = 0; i < 4; i++)
                    printf(INDENT_SPACES "$xxGSV: sat nr %d, elevation: %d, azimuth: %d, snr: %d dbm\n",
                           frame.sats[i].nr,
                           frame.sats[i].elevation,
                           frame.sats[i].azimuth,
                           frame.sats[i].snr);
            }
            else {
                printf(INDENT_SPACES "$xxGSV sentence is not parsed\n");
            }
        } break;

        case MINMEA_SENTENCE_VTG: {
            struct minmea_sentence_vtg frame;
            if (minmea_parse_vtg(&frame, line)) {
                printf(INDENT_SPACES "$xxVTG: true track degrees = %f\n",
                       minmea_tofloat(&frame.true_track_degrees));
                printf(INDENT_SPACES "        magnetic track degrees = %f\n",
                       minmea_tofloat(&frame.magnetic_track_degrees));
                printf(INDENT_SPACES "        speed knots = %f\n",
                       minmea_tofloat(&frame.speed_knots));
                printf(INDENT_SPACES "        speed kph = %f\n",
                       minmea_tofloat(&frame.speed_kph));
            }
            else {
                printf(INDENT_SPACES "$xxVTG sentence is not parsed\n");
            }
        } break;

        //case MINMEA_SENTENCE_GNS: {
            //printf("GNS Sentence\n");
        //} break;

        case MINMEA_INVALID: {
            printf(INDENT_SPACES "$xxxxx sentence is not valid\n");
        } break;

        default: {
            printf(INDENT_SPACES "$xxxxx sentence is not parsed\n");
        } break;
    }
}

static int serial_line_input_byte_mine(unsigned char ch)
{
    line[line_index++] = ch;
    if(line[line_index - 2] == '\r' && line[line_index - 1] == '\n') {
        line[line_index++] = '\0';
        parse_gga(line);
        line_index = 0;
    }
    return 1;
}
/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
PROCESS_BEGIN();
//gps_sensor.configure(SENSORS_HW_INIT, 0);
//SENSORS_ACTIVATE(gps_sensor);
//SENSORS_DEACTIVATE(gps_sensor);
//printf("HELLO");
//cc26xx_uart_set_input(serial_line_input_byte_mine);
//cc26xx_uart_set_input(NULL);
printf("123");
PROCESS_END();
}
