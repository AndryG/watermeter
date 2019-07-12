#pragma once

#include <util/atomic.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>

#define F_CPU	1000000UL

#include "lib.h"
#include "hal.h"
#include "itoa.h"

/* "tp2" период равный 1 секунде
 * „тобы получить к-во тиков - возведи двойку в степень: 1 << tp2_bysec
 * ƒл€ установки периода "два раза в секунду" уменьшить TP2_BY_SEC на единицу, если он сам не равен единице
 */
#define TP2_BY_SEC    3 // 3 - каждый 8 тик - секунда
// максимальный период опроса два раза в сек.
#define TP2_MAX       (TP2_BY_SEC - 1)

// јвтомат канала. ƒопустимое к-во считываний zero на импульс до изменени€ status на CH_ZERO
#define DZS_RANGE_MAX 3
// ћинимальное допустимое DSS (к-во считываний на темный сектор /30% 60∞/) до ускорени€ опроса
#define DSS_RANGE_MIN 2
// ћаксимально допустимое DS (к-во считываний между импульсами /50% 180∞/) до замедлени€ опроса
#define DS_RANGE_MAX  ((1 << TP2_BY_SEC) * 3)


#define CH_ACTIVE 0
#define CH_WAIT   1 // wait (активный, но не в этом тике)
#define CH_ZERO   2 // dzs DurationZeroState превысил порог

typedef struct ch_s {
  /* 0 - active, 1 - ; в этих состо€ни€х проводитс€ чтение в sensorTask
  3 - не придумал */
  u8 status;
  u8 tp2; // период опроса
  u8 sensor;    // цифровое состо€ние сенсора (0-1-2-3)
  u8 ledPower;  // уровень питани€ светодиода (1..3, 0 - выкл)
  //  u32 pulseCount; // к-во импульсов
  u8 state;     // состо€ние автомата обработки
  u32 value;
  u8 dzs;
  u8 dss;
  u8 ds;
} ch_t;

#include "disk.h"

#define	QT_TASK_COUNT	    10

//#define QT_DELAY_SIZE_16
#include "dispatcher.h"

// —ери€ ј÷ѕ считываний по списку каналов мультиплексора
#define ADC_CHANNEL_COUNT   3
#define ADC_CHANNEL_LIST    {0, 1, 2}
#define ADC_ADMUX_INIT      ((0<<REFS1)|(1<<REFS0)|(0<<ADLAR))
//TODO добавить ADC_ADPS предделитель тактовой частоты ADCSRA[2:0]
//#include "adc.h"

#define UART_BAUD_RATE    UART_BAUD_SELECT_DOUBLE_SPEED(38400, F_CPU)
#define UART_RX0_BUFFER_SIZE 0
#define UART_TX0_BUFFER_SIZE 64
#include "uart.lib/uart.h"

//#include "nrf24.lib/src/nrf24l01.h"



#include "snr.h"

#include "tp2.h"

#define DBG_OK            0
#define DBG_LOST_ISR      1



