sx1276 LoRa radio implementation for the canary board.

File descriptions
    1. lora-radio-arch-*
        - Files used to integrate LoRa radio into the contiki netstack.

    2. lora-sx1276* & lora-radio*
        - Files provided by SEMTECH for their STM32L151CB based boards
        - Changes were made to the following files...
              1. lora-sx1276.c
                  - Added uint8_t ioid parameter to interrupt functions
                  - Deleted sx1276.spi.spi check at start of SX1276SetModem function
              2. lora-sx1276-board.h
                  - Changed REG_LR_PAYLOADMAXLENGTH to 0xff from 0x40

    3. lora-support*
        - Files used to implement STM32L151CB mcu drivers using the CC2650 mcu
        - Takes STM32L151CB api calls (gpio, spi, etc) and maps to CC2650 calls
