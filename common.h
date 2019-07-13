#pragma once

#include "lib.h"
#include "itoa.h"
#include "hal.h"

/* ��� ��� �������� �����
 */
typedef u8 tick_t;

/** ����� ���������� */
extern volatile u8 isr;
#define isr_TICK  0
#define isr_T0    1
#define isr_NRF   2

//extern tick_t ticks;

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


void t2_initOcrMode(u8 ticks);

void t2_setOCRandWait(u8 ticks);

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

/* ������� ��������� T2 */
bool t2_isBusy();