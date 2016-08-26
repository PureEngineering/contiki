CURRENT_DIR=$(dirname -- "$(readlink -e -- "$BASH_SOURCE")")/
CONTIKI=$CURRENT_DIR/../../../
STM32L1_CPU=$CONTIKI/cpu/arm/stm32l1/
LORA_PLATFORM=$CONTIKI/platform/loramote/
STM32_COMMON=$CONTIKI/cpu/stm32_common/
OBJ_DIR=$CURRENT_DIR/obj_loramote_bootloader/
GCC_BINS=$CONTIKI/tools/stm32/gcc-arm-none-eabi/bin/

rm -rf $OBJ_DIR
mkdir $OBJ_DIR
cd $OBJ_DIR

$GCC_BINS/arm-none-eabi-gcc \
  -std=c99 -c -mcpu=cortex-m3 -mthumb -Os -g2 -Wall -ffunction-sections -fdata-sections \
  -I$LORA_PLATFORM/LoRaMac/peripherals \
  -I$LORA_PLATFORM/LoRaMac/radio \
  -I$LORA_PLATFORM/LoRaMac/system \
  -I$LORA_PLATFORM/LoRaMac/board \
  -I$LORA_PLATFORM/LoRaMac/board/usb/dfu/inc \
  -I$STM32L1_CPU \
  -I$STM32L1_CPU/STM32L1xx_StdPeriph_Driver/inc \
  -I$CONTIKI/cpu/arm/common/CMSIS \
  -I$STM32_COMMON \
  -I$STM32_COMMON/STM32_USB-FS-Device_Driver/inc \
  -DUSE_DEBUGGER -DUSE_NO_TIMER \
  $CURRENT_DIR/loramote_bootloader.c \
  $LORA_PLATFORM/LoRaMac/peripherals/gpio-ioe.c \
  $LORA_PLATFORM/LoRaMac/peripherals/sx1509.c \
  $LORA_PLATFORM/LoRaMac/peripherals/sx9500.c \
  $LORA_PLATFORM/LoRaMac/system/gpio.c \
  $LORA_PLATFORM/LoRaMac/system/i2c.c \
  $LORA_PLATFORM/LoRaMac/board/gpio-board.c \
  $LORA_PLATFORM/LoRaMac/board/i2c-board.c \
  $LORA_PLATFORM/LoRaMac/board/usb-dfu-board.c \
  $LORA_PLATFORM/LoRaMac/board/usb/dfu/src/dfu_mal.c \
  $LORA_PLATFORM/LoRaMac/board/usb/dfu/src/flash_if.c \
  $LORA_PLATFORM/LoRaMac/board/usb/dfu/src/usb_desc.c \
  $LORA_PLATFORM/LoRaMac/board/usb/dfu/src/usb_istr.c \
  $LORA_PLATFORM/LoRaMac/board/usb/dfu/src/usb_prop.c \
  $LORA_PLATFORM/LoRaMac/board/usb/dfu/src/usb_pwr.c \
  $STM32L1_CPU/system_stm32l1xx.c \
  $STM32L1_CPU/arm-gcc/startup_stm32l1xx_md.s \
  $STM32L1_CPU/STM32L1xx_StdPeriph_Driver/src/misc.c \
  $STM32L1_CPU/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_dbgmcu.c \
  $STM32L1_CPU/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_dma.c \
  $STM32L1_CPU/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_exti.c \
  $STM32L1_CPU/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_flash.c \
  $STM32L1_CPU/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_flash_ramfunc.c \
  $STM32L1_CPU/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_gpio.c \
  $STM32L1_CPU/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_i2c.c \
  $STM32L1_CPU/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_pwr.c \
  $STM32L1_CPU/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_rcc.c \
  $STM32L1_CPU/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_rtc.c \
  $STM32L1_CPU/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_spi.c \
  $STM32L1_CPU/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_syscfg.c \
  $STM32L1_CPU/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_tim.c \
  $STM32L1_CPU/STM32L1xx_StdPeriph_Driver/src/stm32l1xx_usart.c \
  $STM32_COMMON/sysIrqHandlers.c \
  $STM32_COMMON/STM32_USB-FS-Device_Driver/src/usb_core.c \
  $STM32_COMMON/STM32_USB-FS-Device_Driver/src/usb_init.c \
  $STM32_COMMON/STM32_USB-FS-Device_Driver/src/usb_int.c \
  $STM32_COMMON/STM32_USB-FS-Device_Driver/src/usb_mem.c \
  $STM32_COMMON/STM32_USB-FS-Device_Driver/src/usb_regs.c \
  $STM32_COMMON/STM32_USB-FS-Device_Driver/src/usb_sil.c

$GCC_BINS/arm-none-eabi-gcc \
  -mcpu=cortex-m3 -mthumb --specs=nano.specs -Os -g2 -g -nostartfiles -Wl,--gc-sections \
  -L$STM32L1_CPU/arm-gcc -Wl,-T$STM32L1_CPU/arm-gcc/stm32l1xx_md_flash_bootloader.ld -Wl,-Map=bootloader.map \
  -o loramote_bootloader.elf dfu_mal.o flash_if.o gpio-board.o gpio-ioe.o gpio.o i2c-board.o i2c.o loramote_bootloader.o misc.o \
	startup_stm32l1xx_md.o stm32l1xx_dbgmcu.o stm32l1xx_dma.o stm32l1xx_exti.o stm32l1xx_flash.o stm32l1xx_flash_ramfunc.o \
	stm32l1xx_gpio.o stm32l1xx_i2c.o stm32l1xx_pwr.o stm32l1xx_rcc.o stm32l1xx_rtc.o stm32l1xx_spi.o stm32l1xx_syscfg.o \
	stm32l1xx_tim.o stm32l1xx_usart.o sx1509.o sx9500.o sysIrqHandlers.o system_stm32l1xx.o usb_core.o usb_desc.o \
	usb-dfu-board.o usb_init.o usb_int.o usb_istr.o usb_mem.o usb_prop.o usb_pwr.o usb_regs.o usb_sil.o -lc -lm
$GCC_BINS/arm-none-eabi-objcopy -O binary $OBJ_DIR/loramote_bootloader.elf $CURRENT_DIR/loramote_bootloader.bin

rm -rf $OBJ_DIR


#TROUBLESHOOTING
#Compile & link process good
#Problem: usb_pwr.c > Suspend > __WFI()    (wait for interrupt)
#Interrupt is handled in usb-dfu-board.c > USB_FS_WKUP_IRQHandler, but board halts
#Reinitialize first? Or problem with GCC toolchain since builds perfectly on Windows?
#Solved: compile problem since gcc-arm-none-eabi v4.9 --> use v4.8
