#ifndef __DISPATCHER_H
#define __DISPATCHER_H

#include <stdint.h>
#include <stdbool.h>

// тип для задержки выполнения. Можно увеличить директивой QT_DELAY_SIZE_16 (по ум. 8 бит)
#ifdef QT_DELAY_SIZE_16
  typedef uint16_t qtDelay;
#else
  typedef uint8_t qtDelay;
#endif

// указатель на функцию задачи
typedef void(*qtTaskPtr)(void);

void qtDecrementDelay(void);
void qtDispatch(void);
bool qtDispatch2(void);
void qtInit(void);
void qtTask(qtTaskPtr ptr, qtDelay tick);

#endif
