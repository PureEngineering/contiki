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
#include <stdio.h>
#include <errno.h>
#include "contiki.h"
/*---------------------------------------------------------------------------*/
/* Register name faking - works in collusion with the linker */
register char *stack_ptr asm ("sp");
extern int errno;
/*---------------------------------------------------------------------------*/
extern int __io_putstring(const unsigned char *buffer, int size);
extern int __io_putchar(int ch);
/*---------------------------------------------------------------------------*/
#if defined(REDIRECT_STDIO)
/*---------------------------------------------------------------------------*/
/* Write a character to a file. 'libc' subroutines will use this system routine
   for output to all files, including stdout. Returns number of bytes sent */
size_t _write(int handle, const unsigned char *buffer, size_t size)
{
  int data_idx;

  #ifdef REDIRECT_STDIO_STRINGMODE
    __io_putstring(buffer, size);
  #else
    for(data_idx = 0; data_idx < size; data_idx++) {
      __io_putchar(*buffer++);
    }
  #endif
  return size;
}
/*---------------------------------------------------------------------------*/
caddr_t _sbrk(int incr)
{
  extern char end;  // Defined by the linker
  static char *heap_end;
  char *prev_heap_end;

  if(heap_end == 0) {
    heap_end = &end;
  }
  prev_heap_end = heap_end;
  if(heap_end + incr > stack_ptr) {
    _write(1, "Heap and stack collision\n", 25);
    //abort();
    errno = ENOMEM;
    return (caddr_t)-1;
  }

  heap_end += incr;
  return (caddr_t)prev_heap_end;
}
/*---------------------------------------------------------------------------*/
#endif /* REDIRECT_STDIO */
/*---------------------------------------------------------------------------*/
