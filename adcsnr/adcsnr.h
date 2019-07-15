#pragma once

#define F_CPU 8000000u

#include "../common/hal.h"

#define TP2_BY_SEC    6

// Серия АЦП считываний по списку каналов мультиплексора
#define ADC_CHANNEL_COUNT   2
#define ADC_CHANNEL_LIST    {0, 1}
#define ADC_ADMUX_INIT      (0 << REFS1 | 1 << REFS0 | 0 << ADLAR)