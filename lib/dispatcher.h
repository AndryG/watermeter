#ifndef __DISPATCHER_H
#define __DISPATCHER_H
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// размер очереди задач (в штуках)
#ifndef QT_TASK_COUNT
  #error Need define QT_TASK_COUNT
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

void qtDecrementDelay(void);
//extern qtDelay qtDecrementDelay2(qtDelay tick);
void qtDispatch(void);
bool qtDispatch2(void);
void qtInit(void);
void qtTask(qtTaskPtr ptr, qtDelay tick);

#endif
