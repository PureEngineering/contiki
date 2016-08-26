/*---------------------------------------------------------------------*/
#include "contiki.h"
#include "dev/leds.h"
/*---------------------------------------------------------------------*/
PROCESS (blinkingleds_process, "Blinking LEDs");
AUTOSTART_PROCESSES(&blinkingleds_process);
/*---------------------------------------------------------------------*/
static struct etimer et;
/*---------------------------------------------------------------------*/
PROCESS_THREAD(blinkingleds_process, ev, data)
{
	PROCESS_BEGIN();
	
	etimer_set(&et, CLOCK_SECOND*.25);
	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

	while(1){
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
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------*/
