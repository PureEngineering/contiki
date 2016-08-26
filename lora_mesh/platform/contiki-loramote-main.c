/*
 * Copyright (c) 2005, Swedish Institute of Computer Science
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
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */
/*---------------------------------------------------------------------------*/
#include "lora-contiki-interface.h"
/*---------------------------------------------------------------------------*/
SENSORS(&button_sensor,
        &radio_sensor,
        &temperature_sensor,
        &altitude_sensor,
        &pressure_sensor,
        &battery_sensor);
/*---------------------------------------------------------------------------*/
extern unsigned char node_mac[8];
static linkaddr_t rime_addr;
static uip_ipaddr_t ipaddr;
/*---------------------------------------------------------------------------*/
static void print_processes(struct process * const processes[]);
static void print_device_config(void);
static void set_rime_addr(void);
extern bool Virtual_ComPort_IsOpen(void);
/*---------------------------------------------------------------------------*/
int main(int argc, char **argv)
{
  /* Initialize hardware */
  BoardInitMcu_Contiki();
  leds_init();
  rtimer_init();
  serial_line_arch_init();
  printf("\nInitializing hardware... Done!\n");


  /* Initialize Contiki */
  printf("Initializing Contiki... "); fflush(stdout);
  clock_init();
  watchdog_init();
  process_init();
  process_start(&etimer_process, NULL);
  ctimer_init();
  serial_line_init();
  process_start(&sensors_process, NULL);
  printf("Done!\n");


  /* Initialize networking */
  printf("Initializing network... "); fflush(stdout);

  /* Restore node id if such has been stored in external mem */
#ifdef NODEID
  node_id = NODEID;
#else/* NODE_ID */
  node_id_restore(); /* also configures node_mac[] */
#endif /* NODE_ID */

  set_rime_addr();
  random_init(node_id);
  netstack_init();

#if UIP_CONF_IPV6
  memcpy(&uip_lladdr.addr, node_mac, sizeof(uip_lladdr.addr));
  queuebuf_init();
  process_start(&tcpip_process, NULL);
  uip_ipaddr_t ipaddr;
  uip_ip6addr(&ipaddr, 0xfc00, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
#endif /* UIP_CONF_IPV6 */

  printf("Done!\n");


  /* Initialize energy estimation */
  energest_init();
  ENERGEST_ON(ENERGEST_TYPE_CPU);


  /* Start user processes */
  print_device_config();
  leds_off(LEDS_ALL);
  printf("----------[ Running %s on LoRaMote ]----------\n\n", CONTIKI_VERSION_STRING);
  print_processes(autostart_processes);
  autostart_start(autostart_processes);


  /* Start the process scheduler loop */
  watchdog_start();
  while(1) {

    int r;
    do {
      watchdog_periodic();			// Reset watchdog
      r = process_run();
    } while(r > 0);

    /* Avoid LPM when a device is connected to serial I/O */
    if(process_nevents() == 0 && !Virtual_ComPort_IsOpen()){
      ENERGEST_OFF(ENERGEST_TYPE_CPU);
      ENERGEST_ON(ENERGEST_TYPE_LPM);
      watchdog_stop();
      lpm_enter_stopmode();			// Enter LPM: Stop mode with RTC
      watchdog_start();
      ENERGEST_OFF(ENERGEST_TYPE_LPM);
      ENERGEST_ON(ENERGEST_TYPE_CPU);
    }

  }

  return 0;
}
/*---------------------------------------------------------------------------*/
static void print_processes(struct process * const processes[])
{
  /* const struct process * const * p = processes; */
  printf("Starting");
  while(*processes != NULL) {
    printf(" '%s'", (*processes)->name);
    processes++;
  }
  printf("\n");
}
/*---------------------------------------------------------------------------*/
static void print_device_config(void)
{
  int i;
  uint8_t longaddr[8];
  uint16_t shortaddr;

  printf("Rime started with address ");
  for(i = 0; i < sizeof(rime_addr.u8) - 1; i++) {
    printf("%d.", rime_addr.u8[i]);
  }
  printf("%d\n", rime_addr.u8[i]);

  shortaddr = (linkaddr_node_addr.u8[0] << 8) + linkaddr_node_addr.u8[1];
  memset(longaddr, 0, sizeof(longaddr));
  linkaddr_copy((linkaddr_t *)&longaddr, &linkaddr_node_addr);
  printf("MAC %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x  ",
	longaddr[0], longaddr[1], longaddr[2], longaddr[3],
	longaddr[4], longaddr[5], longaddr[6], longaddr[7]);

  if(node_id) {
    printf("Node id is set to %u\n", node_id);
  }else{
    printf("Node id not set.\n");
  }

  printf("%s, %s, radio frequency %iMHz\n",
	NETSTACK_MAC.name, NETSTACK_RDC.name, RF_FREQUENCY/1000000);

#if UIP_CONF_IPV6
  printf("Tentative link-local IPv6 address ");
  {
    uip_ds6_addr_t *lladdr;
    int i;
    lladdr = uip_ds6_get_link_local(-1);
    for(i = 0; i < 7; ++i) {
      printf("%02x%02x:", lladdr->ipaddr.u8[i * 2], lladdr->ipaddr.u8[i * 2 + 1]);
    }
    printf("%02x%02x\n", lladdr->ipaddr.u8[14], lladdr->ipaddr.u8[15]);
  }

  if(!UIP_CONF_IPV6_RPL) {
    printf("Tentative global IPv6 address ");
    for(i = 0; i < 7; ++i) {
      printf("%02x%02x:", ipaddr.u8[i * 2], ipaddr.u8[i * 2 + 1]);
    }
    printf("%02x%02x\n", ipaddr.u8[7 * 2], ipaddr.u8[7 * 2 + 1]);
  }
#endif /* UIP_CONF_IPV6 */
}
/*---------------------------------------------------------------------------*/
static void set_rime_addr(void)
{
  memset(&rime_addr, 0, sizeof(linkaddr_t));
#if UIP_CONF_IPV6
  memcpy(rime_addr.u8, node_mac, sizeof(rime_addr.u8));
#else
  if(node_id == 0) {
    for(int i = 0; i < sizeof(linkaddr_t); ++i) {
      rime_addr.u8[i] = node_mac[7 - i];
    }
  } else {
    rime_addr.u8[0] = node_id & 0xff;
    rime_addr.u8[1] = node_id >> 8;
  }
#endif /* UIP_CONF_IPV6 */
  linkaddr_set_node_addr(&rime_addr);
}
/*---------------------------------------------------------------------------*/
