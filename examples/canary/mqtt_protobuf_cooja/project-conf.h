/*
 * Copyright (c) 2014, Texas Instruments Incorporated - http://www.ti.com/
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
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*---------------------------------------------------------------------------*/
#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_
/*---------------------------------------------------------------------------*/
/* Change to match your configuration */
#define IEEE802154_CONF_PANID            0xABCD
#define RF_CORE_CONF_CHANNEL                 25
#define RF_BLE_CONF_ENABLED                   0
/*---------------------------------------------------------------------------*/
/* Enable/Disable Components of this Demo */
#define CC26XX_WEB_DEMO_CONF_MQTT_CLIENT      1

// Serial/spi bootloader
#define ROM_BOOTLOADER_ENABLE                 1
/*---------------------------------------------------------------------------*/
/*
 * Shrink the size of the uIP buffer, routing table and ND cache.
 * Set the TCP MSS
 */
#define UIP_CONF_BUFFER_SIZE                900
#define NBR_TABLE_CONF_MAX_NEIGHBORS          8
#define UIP_CONF_MAX_ROUTES                   8
#define UIP_CONF_TCP_MSS                    128


// #define UIP_DS6_CONF_PERIOD        CLOCK_SECOND
// #define UIP_CONF_TCP                          0
#define RPL_CONF_LEAF_ONLY                    0
#ifndef UIP_CONF_IPV6_RPL
#define UIP_CONF_IPV6_RPL               1
#endif /* UIP_CONF_IPV6_RPL */
/*---------------------------------------------------------------------------*/

#ifndef WITH_NON_STORING
#define WITH_NON_STORING 0 /* Set this to run with non-storing mode */
#endif /* WITH_NON_STORING */

#undef NBR_TABLE_CONF_MAX_NEIGHBORS
#undef UIP_CONF_MAX_ROUTES

#ifdef TEST_MORE_ROUTES
/* configure number of neighbors and routes */
#define NBR_TABLE_CONF_MAX_NEIGHBORS     10
#define UIP_CONF_MAX_ROUTES   30
#else
/* configure number of neighbors and routes */
#define NBR_TABLE_CONF_MAX_NEIGHBORS     10
#define UIP_CONF_MAX_ROUTES   10
#endif /* TEST_MORE_ROUTES */

#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     nullrdc_driver
#undef NULLRDC_CONF_802154_AUTOACK
#define NULLRDC_CONF_802154_AUTOACK       1

/* Define as minutes */
#define RPL_CONF_DEFAULT_LIFETIME_UNIT   60

/* 10 minutes lifetime of routes */
#define RPL_CONF_DEFAULT_LIFETIME        10

#define RPL_CONF_DEFAULT_ROUTE_INFINITE_LIFETIME 1

#if WITH_NON_STORING
#undef RPL_NS_CONF_LINK_NUM
#define RPL_NS_CONF_LINK_NUM 40 /* Number of links maintained at the root. Can be set to 0 at non-root nodes. */
#undef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES 0 /* No need for routes */
#undef RPL_CONF_MOP
#define RPL_CONF_MOP RPL_MOP_NON_STORING /* Mode of operation*/
#endif /* WITH_NON_STORING */
#endif /* PROJECT_CONF_H_ */
/*---------------------------------------------------------------------------*/
