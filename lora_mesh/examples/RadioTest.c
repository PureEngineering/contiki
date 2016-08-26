/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *
 * -----------------------------------------------------------------
 *
 * Simple application to indicate connectivity between two nodes:
 *
 * - Red led indicates a packet sent via radio (one packet sent each second)
 * - Green led indicates that this node can hear the other node but not
 *   necessary vice versa (unidirectional communication).
 * - Yellow led indicates that both nodes can communicate with each
 *   other (bidirectional communication)
 *
 */
/*---------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "contiki.h"
#include "net/rime/rime.h"
#include "dev/leds.h"
/*---------------------------------------------------------------------*/
PROCESS(radiotest_process, "Radio Test");
AUTOSTART_PROCESSES(&radiotest_process);
/*---------------------------------------------------------------------*/
#define ON 1
#define OFF 0

#define HEADER "RTST"
#define PACKET_SIZE 20
#define PORT 9345
#define SEND_FREQUENCY CLOCK_SECOND
/*---------------------------------------------------------------------*/
struct indicator {
	int onoff;
	int led;
	clock_time_t blink_time;
	struct etimer timer;
};
static struct etimer send_beacon_timer;
static struct etimer recv_expired_timer;
static struct indicator send, recv, link;
static uint8_t recv_valid;
/*---------------------------------------------------------------------*/
static void setIndicator(struct indicator *indicator, int onoff) {

	indicator->onoff = onoff;
	if(onoff) {
		leds_on(indicator->led);
		etimer_set(&indicator->timer, indicator->blink_time);
	} else {
		leds_off(indicator->led);
	}
}
/*---------------------------------------------------------------------------*/
static void abc_recv(struct abc_conn *c) {
	/* Packet received */
	if(packetbuf_datalen() < PACKET_SIZE || strncmp((char *)packetbuf_dataptr(), HEADER, sizeof(HEADER))) {
		/* Invalid message */

	} else {
		PROCESS_CONTEXT_BEGIN(&radiotest_process);
		setIndicator(&recv, ON);
		setIndicator(&link, ((char *)packetbuf_dataptr())[sizeof(HEADER)] ? ON : OFF);
		recv_valid = 1;
		etimer_set(&recv_expired_timer, SEND_FREQUENCY);

		/* Synchronize the sending to keep the nodes from sending simultaneously */
		etimer_set(&send_beacon_timer, SEND_FREQUENCY);
		etimer_adjust(&send_beacon_timer, - (int) (SEND_FREQUENCY >> 1));
		PROCESS_CONTEXT_END(&radiotest_process);
	}
}
static const struct abc_callbacks abc_call = {abc_recv};
static struct abc_conn abc;
/*---------------------------------------------------------------------*/
PROCESS_THREAD(radiotest_process, ev, data)
{
	PROCESS_BEGIN();

	/* Initialize the indicators */
	send.onoff = recv.onoff = link.onoff = OFF;
	send.blink_time = recv.blink_time = CLOCK_SECOND*0.1;
	link.blink_time = SEND_FREQUENCY*1.2;
	send.led = LEDS_RED;
	recv.led = LEDS_GREEN;
	link.led = LEDS_YELLOW;

	abc_open(&abc, PORT, &abc_call);
	etimer_set(&send_beacon_timer, SEND_FREQUENCY);

	while(1) {
		PROCESS_WAIT_EVENT();
		if(ev == PROCESS_EVENT_TIMER) {
			if(data == &send_beacon_timer) {
				etimer_reset(&send_beacon_timer);

				/* Send packet */
				packetbuf_copyfrom(HEADER, sizeof(HEADER));
				((char *)packetbuf_dataptr())[sizeof(HEADER)] = recv_valid;
				/* Send arbitrary data to fill the packet size */
				packetbuf_set_datalen(PACKET_SIZE);
				setIndicator(&send, ON);
				abc_send(&abc);

			} else if(data == &link.timer) {
				setIndicator(&link, OFF);

			} else if(data == &recv.timer) {
				setIndicator(&recv, OFF);

			} else if(data == &send.timer) {
				setIndicator(&send, OFF);

			} else if(data == &recv_expired_timer) {
				recv_valid = 0;
			}
		}
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------*/
