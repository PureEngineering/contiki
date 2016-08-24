
#include "lora-sx1276-support.h"

#include "board.h"
#include "ti-lib.h"

#include "gpio-interrupt.h"

#include "board-spi.h"






/*
  Requred GPIO Functions for sx driver
*/
void GpioInit( Gpio_t *obj, int pin, PinModes mode,  PinConfigs config, PinTypes type, uint32_t value ){
  obj->pin = pin;

  if(mode == PIN_OUTPUT){
    ti_lib_ioc_pin_type_gpio_output(pin);
    ti_lib_gpio_write_dio(1<<pin,value);
  }


  if(mode == PIN_INPUT)
    ti_lib_ioc_pin_type_gpio_input(pin);
}

#define interrupt_config (IOC_IOMODE_NORMAL | IOC_WAKE_ON_HIGH | IOC_RISING_EDGE |\
                          IOC_INT_ENABLE | IOC_NO_IOPULL | IOC_INPUT_ENABLE |\
                          IOC_HYST_DISABLE | IOC_SLEW_DISABLE | IOC_CURRENT_2MA |\
                          IOC_STRENGTH_AUTO)

void GpioSetInterrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler ){

  if(obj->pin==IOID_UNUSED)
    return;

  ti_lib_gpio_clear_event_dio(obj->pin);

  ti_lib_ioc_port_configure_set(obj->pin, IOC_PORT_GPIO, interrupt_config);

  gpio_interrupt_register_handler(obj->pin, *irqHandler);

  ti_lib_ioc_int_enable(obj->pin);
}

void GpioRemoveInterrupt( Gpio_t *obj ){

}

void GpioWrite( Gpio_t *obj, uint32_t value ){
  ti_lib_gpio_write_dio(obj->pin,value);
}

uint32_t GpioRead( Gpio_t *obj ){
  return ti_lib_gpio_read_dio(obj->pin);
}

/* END GPIO ****************************************************************/

/*
  Requred SPI Functions for sx driver
*/

uint16_t SpiInOut( Spi_t *obj, uint16_t outData ){

    uint8_t buf = (uint8_t)outData;
    uint32_t rbuf = 0;

    board_spi_open(4000000,BOARD_IOID_SPI_CLK_FLASH);
    board_spi_flush();

    if(!ti_lib_rom_ssi_data_put_non_blocking(SSI0_BASE, buf)) {
      /* Error */
      printf("PUT FAILED\n");
      return false;
    }
    ti_lib_rom_ssi_data_get(SSI0_BASE, &rbuf );

    board_spi_close();

    return (uint16_t)rbuf;
}

/* END SPI *****************************************************************/

/*
  Required Timer functions
*/

void TimerInit( TimerEvent_t *obj, void ( *callback )( void *) ){
    obj->Callback = callback;
}

void TimerStart( TimerEvent_t *obj ){
  ctimer_set(&(obj->timer),128 * obj->Timestamp,obj->Callback,NULL);
}

void TimerStop( TimerEvent_t *obj ){
  ctimer_stop(&(obj->timer));
}

void TimerSetValue( TimerEvent_t *obj, uint32_t value ){
    obj->Timestamp = value;
}

/* END TIMER ***************************************************************/
