/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: Target board general functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project
*/

#include "lora-contiki-interface.h"


/* Initializes the unused GPIO to a know status */
static void BoardUnusedIoInit(void);

/* Flag to indicate if the MCU is initialized */
static bool McuInitialized = false;


void myUartSend(uint8_t data[]){
  int len = 0;
  while(data[len] != '\0'){len++;}
  while(UartPutBuffer(&UartUsb, data, len));
}

void Delay(float s)
{
    DelayMs(s*1000);
}

void DelayMs(uint32_t ms)
{
  rtimer_clock_t delayValue = 0;
  rtimer_clock_t timeout = 0;

  delayValue = (rtimer_clock_t) (ms * (RTIMER_ARCH_SECOND/1000));

  /* Wait delay ms */
  timeout = rtimer_arch_now();
  while((rtimer_arch_now()-timeout) < delayValue)
  {
    __NOP();
  }
}


void BoardInitMcu_Contiki( void )
{
    if( McuInitialized == false )
    {
#if defined( USE_BOOTLOADER )
        /* Set the Vector Table base location at 0x3000 */
        NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x3000 );
#endif
        /* We use IRQ priority group 4 for the entire project */
        /* When setting the IRQ, only the preemption priority is used */
        NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

        /* Disable Systick */
        SysTick->CTRL  &= ~SysTick_CTRL_TICKINT_Msk;    // Systick IRQ off 
        SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;            // Clear SysTick Exception pending flag

        I2cInit( &I2c, I2C_SCL, I2C_SDA );
        AdcInit( &Adc, BAT_LEVEL );

#if defined( USE_DEBUG_PINS )
        GpioInit( &DbgPin1, CON_EXT_1, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
        GpioInit( &DbgPin2, CON_EXT_3, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
        GpioInit( &DbgPin3, CON_EXT_7, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
        GpioInit( &DbgPin4, CON_EXT_9, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
#endif

#if defined( USE_USB_CDC )
        {
            Gpio_t usbDM;

            GpioInit( &usbDM, USB_DM, PIN_INPUT, PIN_PUSH_PULL, PIN_PULL_UP, 0 );

            if( GpioRead( &usbDM ) == 0 )
            {
                UsbMcuInit( );
                UartInit( &UartUsb, UART_USB_CDC, NC, NC );
                UartConfig( &UartUsb, RX_TX, 115200, UART_8_BIT, UART_1_STOP_BIT, NO_PARITY, NO_FLOW_CTRL );
            }
            else
            {
                GpioInit( &usbDM, USB_DM, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
            }
        }
#endif

        BoardUnusedIoInit( );
        McuInitialized = true;
    }
}



static void BoardUnusedIoInit( void )
{
    Gpio_t ioPin;
  
    /* External Connector J5 */
#if !defined( USE_DEBUG_PINS )
    GpioInit( &ioPin, CON_EXT_1, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &ioPin, CON_EXT_3, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &ioPin, CON_EXT_7, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &ioPin, CON_EXT_9, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
#endif

    /* USB */
#if !defined( USE_USB_CDC )
    GpioInit( &ioPin, USB_DM, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &ioPin, USB_DP, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
#endif

    GpioInit( &ioPin, BOOT_1, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
     
    GpioInit( &ioPin, CON_EXT_8, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &ioPin, BAT_LEVEL, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
               
    GpioInit( &ioPin, PIN_PB6, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &ioPin, WKUP1, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
     
#if defined( USE_DEBUGGER )
    DBGMCU_Config( DBGMCU_SLEEP, ENABLE );
    DBGMCU_Config( DBGMCU_STOP, ENABLE);
    DBGMCU_Config( DBGMCU_STANDBY, ENABLE);
#else
    DBGMCU_Config( DBGMCU_SLEEP, DISABLE );
    DBGMCU_Config( DBGMCU_STOP, DISABLE );
    DBGMCU_Config( DBGMCU_STANDBY, DISABLE );
    
    GpioInit( &ioPin, JTAG_TMS, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &ioPin, JTAG_TCK, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &ioPin, JTAG_TDI, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &ioPin, JTAG_TDO, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &ioPin, JTAG_NRST, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
#endif    
}

