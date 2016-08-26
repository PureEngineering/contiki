/*---------------------------------------------------------------------*/
#include <stdio.h>
#include "contiki.h"
#include "dev/leds.h"
#include "lib/sensors.h"
#include "dev/button-sensor.h"
#include "dev/temperature-sensor.h"
#include "dev/pressure-sensor.h"
#include "dev/altitude-sensor.h"
/*---------------------------------------------------------------------*/
PROCESS (actionsensors_process, "Action Sensors");
PROCESS (blinkleds_process, "Blink LEDs");
AUTOSTART_PROCESSES(&actionsensors_process, &blinkleds_process);
/*---------------------------------------------------------------------*/
static struct etimer et;
static int blinkLedsReversed = 0;
/*---------------------------------------------------------------------*/
PROCESS_THREAD(actionsensors_process, ev, data)
{
	PROCESS_BEGIN();
	
	SENSORS_ACTIVATE(button_sensor);
	SENSORS_ACTIVATE(temperature_sensor);
	SENSORS_ACTIVATE(altitude_sensor);
	SENSORS_ACTIVATE(pressure_sensor);

	while(1){
		PROCESS_WAIT_EVENT_UNTIL((ev==sensors_event) && (data == &button_sensor));

		blinkLedsReversed = !blinkLedsReversed;

		if(!blinkLedsReversed){
			printf("Blinking LEDs normally\n");
		}else{
			printf("Blinking LEDs reversed\n");
		}

		/* Print current sensor readings */
		printf("Temperature: %i °C\n", temperature_sensor.value(0));
		printf("Altitude: %i m\n", altitude_sensor.value(0));
		printf("Pressure: %i Pa\n", pressure_sensor.value(0));
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------*/
PROCESS_THREAD(blinkleds_process, ev, data)
{
	PROCESS_BEGIN();
	
	etimer_set(&et, CLOCK_SECOND*.5);
	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

	while(1){
		
		if(!blinkLedsReversed){
			/* Blink yellow LED */
			leds_on(LEDS_YELLOW);
			etimer_reset(&et);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
			leds_off(LEDS_YELLOW);

			/* Blink green LED */	
			leds_on(LEDS_GREEN);
			etimer_reset(&et);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
			leds_off(LEDS_GREEN);

			/* Blink red LED */
			leds_on(LEDS_RED);
			etimer_reset(&et);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
			leds_off(LEDS_RED);
		}else{
			/* Blink red LED */
			leds_on(LEDS_RED);
			etimer_reset(&et);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
			leds_off(LEDS_RED);

			/* Blink green LED */
			leds_on(LEDS_GREEN);
			etimer_reset(&et);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
			leds_off(LEDS_GREEN);

			/* Blink yellow LED */
			leds_on(LEDS_YELLOW);
			etimer_reset(&et);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
			leds_off(LEDS_YELLOW);
		}

	}

	PROCESS_END();
}
/*---------------------------------------------------------------------*/
