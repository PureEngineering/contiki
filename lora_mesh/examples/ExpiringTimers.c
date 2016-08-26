/*---------------------------------------------------------------------*/
#include <stdio.h>
#include "contiki.h"
/*---------------------------------------------------------------------*/
PROCESS (expiringtimers_process, "Expiring Timers");
AUTOSTART_PROCESSES(&expiringtimers_process);
/*---------------------------------------------------------------------*/
static struct ctimer ct;
static struct etimer et;
static struct rtimer rt;
/*---------------------------------------------------------------------*/
void ct_callback(void *ptr);
void rtimer_reset(int seconds);
void rt_callback(void *ptr);
/*---------------------------------------------------------------------*/
PROCESS_THREAD(expiringtimers_process, ev, data)
{
	PROCESS_BEGIN();

	/* Set ctimer configuration */
	ctimer_set(&ct, CLOCK_SECOND*3, ct_callback, NULL);

	/* Start rtimer loop */
	rtimer_reset(2);

	/* Set etimer configuration */
	etimer_set(&et, CLOCK_SECOND*7);
	
	while(1){
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		etimer_reset(&et);
		printf("etimer expired\n");
	}

	PROCESS_END();	
}
/*---------------------------------------------------------------------*/
void ct_callback(void *ptr){
	ctimer_reset(&ct);
	printf("ctimer expired\n");
}
/*---------------------------------------------------------------------*/
void rtimer_reset(int seconds){
	int ret = rtimer_set(&rt, RTIMER_SECOND*seconds, 1, (rtimer_callback_t) rt_callback, NULL);
	if(ret){
		printf("Error Timer: %u\n", ret);
	}
}
/*---------------------------------------------------------------------*/
void rt_callback(void *ptr){
	rtimer_reset(2);
	printf("rtimer expired\n");
}
/*---------------------------------------------------------------------*/
