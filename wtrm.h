#pragma once

#include <util/atomic.h>

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

#define SNR_PORT_LED  PORTD
#define SNR_PORT_IN   PORTC
#define SNR_PORT_OUT  PORTC

#define SNR_CH0_IN   PC0 // PC0-PC1
#define SNR_CH0_OUT  PC4
#define SNR_CH0_LED  PD4 // PD4-PD5

#define SNR_CH1_IN   PC2 // PC2-PC3
#define SNR_CH1_OUT  PC6
#define SNR_CH1_LED  PD6 // PD6-PD7

#include "snr.h"

#define T2_1Hz  5
#define T2_2Hz  4
#define T2_4Hz  3
/* Включение/переключение асинхронного режима T2
 * Делитель 3/4/5 32/64/128 4/2/1 Гц
 */
void t2_init(u8 clockSelect);
bool t2_isBusy();


typedef struct chanel_t {
  u8 sensor;    // цифровое состояние сенсора (0-1-2-3)
  //    u8 snrA1, snrA0, snrB1, snrB0; //аналоговые показания на датчике
  u8 ledPower;  // уровень питания светодиода (1..3, 0 - выкл)
  //    u32 pulseCount; // к-во импульсов
  u8 state;     // состояние канала
} chanel_t;
