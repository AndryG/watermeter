#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <util/atomic.h>
//#include <util/delay.h>

typedef uint8_t   u8;
typedef int8_t    i8;
typedef uint16_t  u16;
typedef int16_t   i16;
typedef uint32_t  u32;
typedef int32_t   i32;

//#define delay_us(us)     __delay_cycles((F_CPU / 1000000) * (us));
//#define delay_ms(ms)     __delay_cycles((F_CPU / 1000) * (ms));

//#define delay_ms(x)		_delay_ms(x)

#define bv(x)   _BV(x)

#define IOP_DDR(port)   (*(volatile u8*)(&(port) - 1))
#define IOP_PIN(port)   (*(volatile u8*)(&(port) - 2))

//#define iop_asInput(port, mask) *(volatile u8*)(&(port))) |= (mask);
#define iop_pin(port)   (*(volatile u8*)(&(port) - 2))

inline static void iop_asOutput(volatile u8 *port, u8 mask) {
  volatile uint8_t *ddr = port - 1;
  *ddr |= mask;
}

inline static void iop_asInput(volatile u8 *port, u8 mask) {
  volatile uint8_t *ddr = port - 1;
  *ddr &= ~mask;
}

#define swap(x) __asm__ __volatile__ ("swap %0" : "=r" (x) : "0" (x))

