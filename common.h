#pragma once

#include "lib.h"
#include "itoa.h"
#include "hal.h"

/* Тип для счетчика тиков
 */
typedef u8 tick_t;

/** Флаги прерываний */
extern volatile u8 isr;
#define isr_TICK  0
#define isr_T0    1
#define isr_NRF   2

//extern tick_t ticks;

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


void t2_initOcrMode(u8 ticks);

void t2_setOCRandWait(u8 ticks);

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

/* признак занятости T2 */
bool t2_isBusy();