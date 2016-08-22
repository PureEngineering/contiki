#ifndef __LORA_SX1276_SUPPORT_H__
#define __LORA_SX1276_SUPPORT_H__

#include <stdint.h>

#include "sys/ctimer.h"

#define DelayMs(x)    clock_delay_usec(x * 100)

#define __IO volatile

//Start GPIO
typedef enum
{
    PIN_INPUT = 0,
    PIN_OUTPUT,
    PIN_ALTERNATE_FCT,
    PIN_ANALOGIC
}PinModes;

typedef enum
{
    PIN_NO_PULL = 0,
    PIN_PULL_UP,
    PIN_PULL_DOWN
}PinTypes;

typedef enum
{
    PIN_PUSH_PULL = 0,
    PIN_OPEN_DRAIN
}PinConfigs;

typedef enum
{
    NO_IRQ = 0,
    IRQ_RISING_EDGE,
    IRQ_FALLING_EDGE,
    IRQ_RISING_FALLING_EDGE
}IrqModes;

typedef enum
{
    IRQ_VERY_LOW_PRIORITY = 0,
    IRQ_LOW_PRIORITY,
    IRQ_MEDIUM_PRIORITY,
    IRQ_HIGH_PRIORITY,
    IRQ_VERY_HIGH_PRIORITY
}IrqPriorities;

typedef struct
{
    int  pin;
    uint16_t pinIndex;
    void *port;
    uint16_t portIndex;
}Gpio_t;

typedef void( GpioIrqHandler )( uint8_t );

void GpioInit( Gpio_t *obj, int pin, PinModes mode, PinConfigs config, PinTypes type, uint32_t value );

void GpioSetInterrupt( Gpio_t *obj, IrqModes irqMode, IrqPriorities irqPriority, GpioIrqHandler *irqHandler );

void GpioRemoveInterrupt( Gpio_t *obj );

void GpioWrite( Gpio_t *obj, uint32_t value );

uint32_t GpioRead( Gpio_t *obj );

//Start Spi
struct Spi_s
{
    //SPI_TypeDef *Spi;
    Gpio_t Mosi;
    Gpio_t Miso;
    Gpio_t Sclk;
    Gpio_t Nss;
    uint32_t freq;
};

typedef struct Spi_s Spi_t;

uint16_t SpiInOut( Spi_t *obj, uint16_t outData );
//End Spi

//Start Timer
typedef struct TimerEvent_s
{
    uint32_t Timestamp;         //! Current timer value
    uint32_t ReloadValue;       //! Timer delay value
    bool IsRunning;             //! Is the timer currently running
    void ( *Callback )( void *); //! Timer IRQ callback function
    struct TimerEvent_s *Next;  //! Pointer to the next Timer object.
    struct ctimer timer;
}TimerEvent_t;

void TimerInit( TimerEvent_t *obj, void ( *callback )( void *) );

void TimerStart( TimerEvent_t *obj );

void TimerStop( TimerEvent_t *obj );

void TimerSetValue( TimerEvent_t *obj, uint32_t value );
//End Timer


#endif
