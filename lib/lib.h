#pragma once
#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>

typedef uint8_t   u8;
typedef int8_t    i8;
typedef uint16_t  u16;
typedef int16_t   i16;

//#define delay_us(us)     __delay_cycles((F_CPU / 1000000) * (us));
//#define delay_ms(ms)     __delay_cycles((F_CPU / 1000) * (ms));

#define delay_ms(x)		_deblay_ms(x)

