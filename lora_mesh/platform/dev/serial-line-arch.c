/*
 * Copyright (c) 2012, STMicroelectronics.
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
#include "serial-line-arch.h"
/*---------------------------------------------------------------------------*/
/* Retarget stdio */
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
  #define PUTSTRING_PROTOTYPE int __io_putstring(const unsigned char *buffer, int size)
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE * f)
#endif /* __GNUC__ */
/*---------------------------------------------------------------------------*/
extern bool Virtual_ComPort_IsOpen(void);
/*---------------------------------------------------------------------------*/
void serial_line_arch_input_callback(UartNotifyId_t id){
  watchdog_periodic();
  if(id == UART_NOTIFY_RX){
    uint8_t ch = 0;
    while(UartGetChar(&UartUsb, &ch));
    serial_line_input_byte(ch);
  }
}
/*---------------------------------------------------------------------------*/
void serial_line_arch_init(void)
{
  UartUsb.IrqNotify = serial_line_arch_input_callback;
}
/*---------------------------------------------------------------------------*/
PUTCHAR_PROTOTYPE
{
  watchdog_periodic();
  /* Does not work during hardware interrupts */
  if(Virtual_ComPort_IsOpen()){
    while(UartPutChar(&UartUsb, ch));
  }
  return ch;
}
/*---------------------------------------------------------------------------*/
PUTSTRING_PROTOTYPE
{
  watchdog_periodic();
  if(Virtual_ComPort_IsOpen()){
    while(UartPutBuffer(&UartUsb, (uint8_t*)buffer, size));
  }
  return size;
}
/*---------------------------------------------------------------------------*/
void uart1_set_input(int (*input)(unsigned char c)){}
/*---------------------------------------------------------------------------*/
