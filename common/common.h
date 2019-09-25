#ifndef COMMON_H
#define COMMON_H

#ifndef TP2_BY_SEC
  #error Not define TP2_BY_SEC;
#endif

#include "../lib/dispatcher.h"

#include "../lib/lib.h"
#include "../lib/itoa.h"

#include "hal.h"

#define UART_BAUD_RATE    UART_BAUD_SELECT(38400, F_CPU)
//#define UART_BAUD_RATE    UART_BAUD_SELECT_DOUBLE_SPEED(76800, F_CPU)
#include "../uart.lib/uart.h"

/* Тип для счетчика тиков
 */
typedef u8 tick_t;

/** Флаги прерываний */
extern volatile u8 isr;
#define isr_TICK  0
#define isr_T0    1
#define isr_NRF   2

/* Макрос для указывания TP2 периода относительно TP2_BY_SEC. TP2_BY_SEC - 2  = 1/2^2 = 0,25сек
 * Макрос следит за переполнением значения. Возращает ноль (каждый тик), если указано очень сильное уменьшение
 */
#define TP2_SUB(x)    (TP2_BY_SEC > (x) ? TP2_BY_SEC - (x) : 0)

/* Cчитает min к-во тиков до соблюдения условия T2p
 * Tp2[] - TimePower2 - указание интервала "каждый 2^Tp2 тик" степенью двойки
 */
tick_t tp2_calcDelay(tick_t ticks, uint8_t Tp2);

/* Вариант функции tp2_calcDelay для двух значений
 */
tick_t tp2_calcDelay2(tick_t ticks, uint8_t Tp2A, uint8_t Tp2B);

/* Проверка "значение ticks подходит под требование Tp2 интервала"
 */
bool tp2_test(tick_t ticks, uint8_t Tp2);


void t2_setOvfAndWait(u8 ticks);

void t2_initOvfMode(u8 ticks);

void blink();

void tc0_start();

/* Начало считывания датчика 0
 */
void snr_begin0(u8 ledPower0);

/* Начало считывания датчика 1
 */
void snr_begin1(u8 ledPower1);

/* Чтение цифровых значений с датчиков канала 0
 */
u8 snr_readCh0();

/* Чтение цифровых значений с датчиков канала 1
 */
u8 snr_readCh1();

/* Отключение считывателя. Перевод в режим мин. потребления сразу оба канала.
 * Даже если канал не используется, все равно линии нужно перевести в режим мин. потребления
 */
void snr_off();

#define led_init()    (IOP_DDR(LED_PORT) |= bv(LED_PIN))

/** зажигает led на 0,25 сек */
void led_blink();


#endif