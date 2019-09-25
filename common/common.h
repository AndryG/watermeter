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

/* ��� ��� �������� �����
 */
typedef u8 tick_t;

/** ����� ���������� */
extern volatile u8 isr;
#define isr_TICK  0
#define isr_T0    1
#define isr_NRF   2

/* ������ ��� ���������� TP2 ������� ������������ TP2_BY_SEC. TP2_BY_SEC - 2  = 1/2^2 = 0,25���
 * ������ ������ �� ������������� ��������. ��������� ���� (������ ���), ���� ������� ����� ������� ����������
 */
#define TP2_SUB(x)    (TP2_BY_SEC > (x) ? TP2_BY_SEC - (x) : 0)

/* C������ min �-�� ����� �� ���������� ������� T2p
 * Tp2[] - TimePower2 - �������� ��������� "������ 2^Tp2 ���" �������� ������
 */
tick_t tp2_calcDelay(tick_t ticks, uint8_t Tp2);

/* ������� ������� tp2_calcDelay ��� ���� ��������
 */
tick_t tp2_calcDelay2(tick_t ticks, uint8_t Tp2A, uint8_t Tp2B);

/* �������� "�������� ticks �������� ��� ���������� Tp2 ���������"
 */
bool tp2_test(tick_t ticks, uint8_t Tp2);


void t2_setOvfAndWait(u8 ticks);

void t2_initOvfMode(u8 ticks);

void blink();

void tc0_start();

/* ������ ���������� ������� 0
 */
void snr_begin0(u8 ledPower0);

/* ������ ���������� ������� 1
 */
void snr_begin1(u8 ledPower1);

/* ������ �������� �������� � �������� ������ 0
 */
u8 snr_readCh0();

/* ������ �������� �������� � �������� ������ 1
 */
u8 snr_readCh1();

/* ���������� �����������. ������� � ����� ���. ����������� ����� ��� ������.
 * ���� ���� ����� �� ������������, ��� ����� ����� ����� ��������� � ����� ���. �����������
 */
void snr_off();

#define led_init()    (IOP_DDR(LED_PORT) |= bv(LED_PIN))

/** �������� led �� 0,25 ��� */
void led_blink();


#endif