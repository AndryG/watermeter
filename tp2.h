#ifndef TP2_H
#define TP2_H

/* Тип для счетчика тиков
 */
typedef uint8_t tp2_ticks_t;

/* Cчитает min к-во тиков до соблюдения условия T2p
 * Tp2[] - TimePower2 - указание интервала "каждый 2^Tp2 тик" степенью двойки
 */
tp2_ticks_t tp2_calcDelay(tp2_ticks_t ticks, uint8_t Tp2);

/* Вариант функции tp2_calcDelay для двух значений
 */
tp2_ticks_t tp2_calcDelay2(tp2_ticks_t ticks, uint8_t Tp2A, uint8_t Tp2B);

/* Проверка "значение ticks подходит под требование Tp2 интервала"
 */
bool tp2_test(tp2_ticks_t ticks, uint8_t Tp2);

#endif