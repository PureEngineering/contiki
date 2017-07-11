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
#include "gps_demo.h"
#include "lib/list.h"
#include "lib/sensors.h"
#include "net/ipv6/sicslowpan.h"
#include "sys/process.h"

PROCESS(gps_process, "gps process");
AUTOSTART_PROCESSES(&gps_process);

PROCESS_THREAD(gps_process, ev, data) {
    PROCESS_BEGIN();

    gps_sensor.configure(SENSORS_HW_INIT, 0);
    SENSORS_ACTIVATE(gps_sensor);
    //SENSORS_DEACTIVATE(gps_sensor);

    PROCESS_END();
}
