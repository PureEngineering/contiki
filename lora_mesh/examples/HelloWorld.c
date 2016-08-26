/*---------------------------------------------------------------------*/
#include <stdio.h>
#include "contiki.h"
/*---------------------------------------------------------------------*/
PROCESS (helloworld_process, "Hello World");
AUTOSTART_PROCESSES(&helloworld_process);
/*---------------------------------------------------------------------*/
PROCESS_THREAD(helloworld_process, ev, data)
{
	PROCESS_BEGIN();

	while(1){
		printf("Hello world!\n");
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------*/
