#ifndef __DISPATCHER_H
#define __DISPATCHER_H
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include "../config.h"

// ������ ������� ����� (� ������)
#ifndef QT_TASK_COUNT
  #error not defined QT_TASK_COUNT
#endif

// ��� ��� �������� ����������. ����� ��������� ���������� QT_DELAY_SIZE_16 (�� ��. 8 ���)
#ifdef QT_DELAY_SIZE_16
  typedef uint16_t qtDelay;
#else
  typedef uint8_t qtDelay;
#endif

// ��� ������
#ifndef QT_QUEUE_OVERFLOW_CODE
  #define QT_QUEUE_OVERFLOW_CODE EXIT_FAILURE
#endif

// ��������� �� ������� ������
typedef void(*qtTaskPtr)(void);

extern void qtDecrementDelay(void);
extern void qtDispatch(void);
extern void qtInit(void);
extern void qtTask(qtTaskPtr ptr, qtDelay tick);

#endif
