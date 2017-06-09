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

int
serial_line_input_byte_mine(unsigned char c)
{
    //printf("zo");
    cc26xx_uart_write_byte(c);
    return 1;
}
/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
gps_sensor.configure(SENSORS_HW_INIT, 0);
PROCESS_BEGIN();

//cc26xx_uart_init();

//serial_line_init();

//ti_lib_ioc_pin_type_gpio_input(IOID_2);

printf("HELLO");

cc26xx_uart_set_input(serial_line_input_byte_mine);

printf("saw \n");
while(1) {
    PROCESS_YIELD();
//if(ev == serial_line_event_message)             ///
//    printf("~Received line: %s\n", (char *)data);   ///

}

PROCESS_END();
}
