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
/**
 * \addtogroup sensortag-cc26xx-peripherals
 * @{
 *
 * \file
 *  Sensortag-specific board initialisation driver
 */
/*---------------------------------------------------------------------------*/
#include "contiki-conf.h"
#include "lib/sensors.h"
// #include "buzzer.h"
#include "lpm.h"
#include "ti-lib.h"
#include "board-peripherals.h"
#include "board-i2c.h"

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

/*---------------------------------------------------------------------------*/
static void
power_domains_on(void)
{
  /* Turn on the PERIPH PD */
  ti_lib_prcm_power_domain_on(PRCM_DOMAIN_PERIPH);

  /* Wait for domains to power on */
  while((ti_lib_prcm_power_domain_status(PRCM_DOMAIN_PERIPH)
        != PRCM_DOMAIN_POWER_ON));
}
/*---------------------------------------------------------------------------*/
static void
lpm_wakeup_handler(void)
{
  power_domains_on();
}
/*---------------------------------------------------------------------------*/
static void
shutdown_handler(uint8_t mode)
{
  if(mode == LPM_MODE_SHUTDOWN) {
  //  SENSORS_DEACTIVATE(opt_3001_sensor);
  //  SENSORS_DEACTIVATE(bme_280_sensor);
    SENSORS_DEACTIVATE(gas_sensor);
  //  SENSORS_DEACTIVATE(lis2de12_accel_sensor);
  //  SENSORS_DEACTIVATE(lis3mdl_mag_sensor);
  }

  /* In all cases, stop the I2C */
  board_i2c_shutdown();
}
/*---------------------------------------------------------------------------*/
/*
 * Declare a data structure to register with LPM.
 * We don't care about what power mode we'll drop to, we don't care about
 * getting notified before deep sleep. All we need is to be notified when we
 * wake up so we can turn power domains back on for I2C and SSI, and to make
 * sure everything on the board is off before CM3 shutdown.
 */
LPM_MODULE(canary_module, NULL, shutdown_handler, lpm_wakeup_handler,
           LPM_DOMAIN_NONE);
/*---------------------------------------------------------------------------*/
static void
configure_unused_pins(void)
{
  ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_DIO4);
  ti_lib_ioc_io_port_pull_set(BOARD_IOID_DIO4, IOC_IOPULL_DOWN);

  //GPS MODULE EVERY PIN PULLDOWN
  ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_GPS_WAKEUP);
  ti_lib_ioc_port_configure_set(BOARD_IOID_GPS_WAKEUP,IOC_PORT_GPIO, IOC_IOPULL_DOWN);


  ti_lib_ioc_pin_type_gpio_output(BOARD_IOID_GPS_RESET); //output
  ti_lib_gpio_write_dio(BOARD_IOID_GPS_RESET, 1); //high
  ti_lib_ioc_port_configure_set(BOARD_IOID_GPS_RESET,IOC_PORT_GPIO, IOC_IOPULL_DOWN|
                                                  IOC_IOMODE_OPEN_DRAIN_NORMAL|
                                                  IOC_CURRENT_2MA | IOC_STRENGTH_MIN  );

  ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_GPS_RX); //input
  ti_lib_ioc_port_configure_set(BOARD_IOID_GPS_RX, IOC_PORT_GPIO, IOC_IOPULL_UP); //pull up

  ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_GPS_TX); //input
  ti_lib_ioc_port_configure_set(BOARD_IOID_GPS_TX, IOC_PORT_GPIO, IOC_IOPULL_UP); //pull up

  ti_lib_ioc_pin_type_gpio_output(BOARD_IOID_GPS_ON_OFF); //output
  ti_lib_gpio_write_dio(BOARD_IOID_GPS_ON_OFF, 0); //low
  ti_lib_ioc_port_configure_set(BOARD_IOID_GPS_ON_OFF, IOC_PORT_GPIO,
                              IOC_CURRENT_2MA |  IOC_STRENGTH_MIN);

  ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_GPS_1PPS);
  ti_lib_ioc_port_configure_set(BOARD_IOID_GPS_1PPS, IOC_PORT_GPIO, IOC_IOPULL_DOWN);

  ti_lib_ioc_pin_type_gpio_output(BOARD_IOID_GPS_ENABLE); //output
  ti_lib_gpio_write_dio(BOARD_IOID_GPS_ENABLE, 0); //low
  ti_lib_ioc_port_configure_set(BOARD_IOID_GPS_ENABLE, IOC_PORT_GPIO,
                                  IOC_CURRENT_2MA | IOC_STRENGTH_MIN);
  //end gps

  //pir
  ti_lib_ioc_pin_type_gpio_input(BOARD_IOID_PIR_OUT);
  ti_lib_ioc_port_configure_set(BOARD_IOID_PIR_OUT, IOC_PORT_GPIO, IOC_IOPULL_DOWN);





}
/*---------------------------------------------------------------------------*/
void
board_init()
{
  bool int_disabled = ti_lib_int_master_disable();

  power_domains_on();

  /* Enable GPIO peripheral */
  ti_lib_prcm_peripheral_run_enable(PRCM_PERIPH_GPIO);

  /* Apply settings and wait for them to take effect */
  ti_lib_prcm_load_set();
  while(!ti_lib_prcm_load_get());

  /* I2C controller */
  board_i2c_wakeup();

  // buzzer_init();

  lpm_register_module(&canary_module);

  /* For unsupported peripherals, select a default pin configuration */
  configure_unused_pins();

  /* Re-enable interrupt if initially enabled. */
  if(!int_disabled) {
    ti_lib_int_master_enable();
  }
}
/*---------------------------------------------------------------------------*/
/** @} */