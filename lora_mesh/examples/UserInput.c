/*---------------------------------------------------------------------*/
#include <stdio.h>
#include "contiki.h"
#include "dev/serial-line.h"
/*---------------------------------------------------------------------*/
PROCESS (userinput_process, "User Input");
AUTOSTART_PROCESSES(&userinput_process);
/*---------------------------------------------------------------------*/
char str1[50];
/*---------------------------------------------------------------------*/
PROCESS_THREAD(userinput_process, ev, data)
{
	PROCESS_BEGIN();

	for(;;) {
		PROCESS_YIELD();
		if(ev == serial_line_event_message) {
			printf("received line: %s\n", (char *)data);
		}
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------*/
