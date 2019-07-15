#ifndef __CONFIG_H
#define __CONFIG_H

#include "lib.h"

#define F_CPU 1000000Lu


//#define MODE              M_SNR
//#define MODE              M_ADC
#define MODE              M_MIX
#define M_SNR             1
#define M_ADC             2
#define M_MIX             3

#define	QT_TASK_COUNT	    10

#define QT_DELAY_SIZE_16

//#define UART_BAUD_RATE    UART_BAUD_SELECT(38400, F_CPU)
#define UART_BAUD_RATE    UART_BAUD_SELECT_DOUBLE_SPEED(38400, F_CPU)

#include "itoa.h"
#include "dispatcher.h"
#include "kbd.h"
#include "uart.h"

// Серия АЦП считываний по списку каналов мультиплексора
#define ADC_CHANNEL_COUNT   3
#define ADC_CHANNEL_LIST    {0, 1, 2}
#define ADC_ADMUX_INIT      ((0<<REFS1)|(1<<REFS0)|(0<<ADLAR))
//TODO добавить ADC_ADPS предделитель тактовой частоты ADCSRA[2:0]
#include "adc.h"


// DISK - считывание с диска

// предыдущий символ
extern uint8_t dskPrevCh;

// счетчик повторов последнего вх. символа
extern uint8_t dskSameCnt;

// обработка входящего символа
extern uint8_t dskAutomat(uint8_t ch);


/* PWM FAN*/
void pwmPower(uint8_t power);
uint8_t pwmState();
void pwmPlus(uint8_t diff);
void pwmMinus(uint8_t diff);
#endif
