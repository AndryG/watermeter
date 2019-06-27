#pragma once

#define F_CPU	1000000UL

#include "lib.h"

#define	QT_TASK_COUNT	    10

#define QT_DELAY_SIZE_16
#include "dispatcher.h"

// Серия АЦП считываний по списку каналов мультиплексора
#define ADC_CHANNEL_COUNT   3
#define ADC_CHANNEL_LIST    {0, 1, 2}
#define ADC_ADMUX_INIT      ((0<<REFS1)|(1<<REFS0)|(0<<ADLAR))
//TODO добавить ADC_ADPS предделитель тактовой частоты ADCSRA[2:0]
#include "adc.h"

#define UART_BAUD_RATE    UART_BAUD_SELECT_DOUBLE_SPEED(38400, F_CPU)
//#define UART_RX0_BUFFER_SIZE 128
//#define UART_TX0_BUFFER_SIZE 64
#include "uart.lib/uart.h"

#include "nrf24.lib/src/nrf24l01.h"

