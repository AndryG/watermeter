#pragma once

#define F_CPU	8000000UL

#include "../lib/lib.h"
#include "../common/hal.h"

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

// status chanells
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

#define	QT_TASK_COUNT	    12

