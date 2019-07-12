#pragma once

#include <util/atomic.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>

#define F_CPU	1000000UL

#include "lib.h"
#include "hal.h"
#include "itoa.h"

/* "tp2" ������ ������ 1 �������
 * ����� �������� �-�� ����� - ������� ������ � �������: 1 << tp2_bysec
 * ��� ��������� ������� "��� ���� � �������" ��������� TP2_BY_SEC �� �������, ���� �� ��� �� ����� �������
 */
#define TP2_BY_SEC    3 // 3 - ������ 8 ��� - �������
// ������������ ������ ������ ��� ���� � ���.
#define TP2_MAX       (TP2_BY_SEC - 1)

// ������� ������. ���������� �-�� ���������� zero �� ������� �� ��������� status �� CH_ZERO
#define DZS_RANGE_MAX 3
// ����������� ���������� DSS (�-�� ���������� �� ������ ������ /30% 60�/) �� ��������� ������
#define DSS_RANGE_MIN 2
// ����������� ���������� DS (�-�� ���������� ����� ���������� /50% 180�/) �� ���������� ������
#define DS_RANGE_MAX  ((1 << TP2_BY_SEC) * 3)


#define CH_ACTIVE 0
#define CH_WAIT   1 // wait (��������, �� �� � ���� ����)
#define CH_ZERO   2 // dzs DurationZeroState �������� �����

typedef struct ch_s {
  /* 0 - active, 1 - ; � ���� ���������� ���������� ������ � sensorTask
  3 - �� �������� */
  u8 status;
  u8 tp2; // ������ ������
  u8 sensor;    // �������� ��������� ������� (0-1-2-3)
  u8 ledPower;  // ������� ������� ���������� (1..3, 0 - ����)
  //  u32 pulseCount; // �-�� ���������
  u8 state;     // ��������� �������� ���������
  u32 value;
  u8 dzs;
  u8 dss;
  u8 ds;
} ch_t;

#include "disk.h"

#define	QT_TASK_COUNT	    10

//#define QT_DELAY_SIZE_16
#include "dispatcher.h"

// ����� ��� ���������� �� ������ ������� ��������������
#define ADC_CHANNEL_COUNT   3
#define ADC_CHANNEL_LIST    {0, 1, 2}
#define ADC_ADMUX_INIT      ((0<<REFS1)|(1<<REFS0)|(0<<ADLAR))
//TODO �������� ADC_ADPS ������������ �������� ������� ADCSRA[2:0]
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



