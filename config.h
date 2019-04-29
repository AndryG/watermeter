#ifndef __CONFIG_H
#define __CONFIG_H

#include <ioavr.h>
#include <intrinsics.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint8_t   u8;
typedef int8_t    i8;
typedef uint16_t  u16;
typedef int16_t   i16;

#define bv(x)           (1<<(x))
#define _BV(x)          (1<<(x))
#define delay_us(us)     __delay_cycles((F_CPU / 1000000) * (us));
#define delay_ms(ms)     __delay_cycles((F_CPU / 1000) * (ms));

#define F_CPU 8000000Lu

#define	QT_TASK_COUNT	    10	

#define QT_DELAY_SIZE_16

#define UART_BAUD_RATE      38400

#include "itoa.h"
#include "dispatcher.h"
#include "kbd.h"
#include "uart.h"

// ����� ��� ���������� �� ������ ������� ��������������
#define ADC_CHANNEL_COUNT   3
#define ADC_CHANNEL_LIST    {0, 1, 2}
#define ADC_ADMUX_INIT      ((0<<REFS1)|(1<<REFS0)|(0<<ADLAR))
//TODO �������� ADC_ADPS ������������ �������� ������� ADCSRA[2:0]
#include "adc.h"


// DISK - ���������� � �����

// ���������� ������
extern uint8_t dskPrevCh;

// ������� �������� ���������� ��. �������
extern uint8_t dskSameCnt;

// ��������� ��������� �������
extern uint8_t dskAutomat(uint8_t ch);


/* PWM FAN*/
void pwmPower(uint8_t power);
uint8_t pwmState();
void pwmPlus(uint8_t diff);
void pwmMinus(uint8_t diff);
#endif
