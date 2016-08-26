/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Target board general functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project
*/

#ifndef __LORA_CONTIKI_INTERFACE_H__
#define __LORA_CONTIKI_INTERFACE_H__


/* Contiki core */
#include "contiki.h"
#include "contiki-net.h"
#include "sys/autostart.h"
#include "sys/node-id.h"
#include "dev/leds.h"
#include "dev/serial-line.h"
#include "dev/slip.h"
#include "dev/watchdog.h"
#include "dev/xmem.h"
#include "lib/random.h"

#include "net/netstack.h"
#include "net/ip/uip.h"
#include "net/rime/rime.h"
#include "net/mac/frame802154.h"
#if WITH_UIP6
#include "net/ipv6/uip-ds6.h"
#endif /* WITH_UIP6 */

#include "lib/sensors.h"
#include "dev/button-sensor.h"
#include "dev/radio-sensor.h"


/*LoRa platform */
#include "lpm-arch.h"
#include "board.h"
#include "sx1509.h"
#include "altitude-sensor.h"
#include "battery-sensor.h"
#include "pressure-sensor.h"
#include "temperature-sensor.h"
#include "serial-line-arch.h"
#include "lora-radio-arch.h"


/* Initializes the target board peripherals */
void BoardInitMcu_Contiki(void);

/* Blocking delay of "s" seconds */
void Delay(float s);

/* Blocking delay of "ms" milliseconds */
void DelayMs(uint32_t ms);


#endif // __LORA_CONTIKI_INTERFACE_H__
