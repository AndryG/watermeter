#ifndef __DISPATCHER_H
#define __DISPATCHER_H
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include "../config.h"

// размер очереди задач (в штуках)
#ifndef QT_TASK_COUNT
  #error not defined QT_TASK_COUNT
#endif

// тип для задержки выполнения. Можно увеличить директивой QT_DELAY_SIZE_16 (по ум. 8 бит)
#ifdef QT_DELAY_SIZE_16
  typedef uint16_t qtDelay;
#else
  typedef uint8_t qtDelay;
#endif

// Код выхода
#ifndef QT_QUEUE_OVERFLOW_CODE
  #define QT_QUEUE_OVERFLOW_CODE EXIT_FAILURE
#endif

// указатель на функцию задачи
typedef void(*qtTaskPtr)(void);

extern void qtDecrementDelay(void);
extern void qtDispatch(void);
extern void qtInit(void);
extern void qtTask(qtTaskPtr ptr, qtDelay tick);

#endif
