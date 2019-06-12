#include <ioavr.h>
#include <intrinsics.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#pragma diag_suppress=Pe301
typedef uint8_t   u8;
typedef int8_t    i8;
typedef uint16_t  u16;
typedef int16_t   i16;
#pragma diag_default=Pe301

#define bv(x)           (1<<(x))
#define _BV(x)          (1<<(x))

#define delay_us(us)     __delay_cycles((F_CPU / 1000000) * (us));
#define delay_ms(ms)     __delay_cycles((F_CPU / 1000) * (ms));

