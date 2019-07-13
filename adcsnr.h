#pragma once

#define F_CPU 8000000u

#define TP2_BY_SEC    6

#include "common.h"

#define	QT_TASK_COUNT	    10
#define QT_DELAY_SIZE_16
#include "dispatcher.h"

#define UART_BAUD_RATE    UART_BAUD_SELECT(38400, F_CPU)
//#define UART_BAUD_RATE    UART_BAUD_SELECT_DOUBLE_SPEED(76800, F_CPU)
#include "uart.h"

// Серия АЦП считываний по списку каналов мультиплексора
#define ADC_CHANNEL_COUNT   2
#define ADC_CHANNEL_LIST    {0, 1}
#define ADC_ADMUX_INIT      (0 << REFS1 | 1 << REFS0 | 0 << ADLAR)

#include "adc.h"
