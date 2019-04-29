#ifndef __ADC_H
#define __ADC_H

// ������������� ���������� ADC � ���������� �������

#include "config.h"

#ifndef ADC_CHANNEL_LIST
  #error "Not defined ADC_CHANNEL_LIST - list channels for adc converting {CH_1, CH_2,...}"
#endif

#ifndef ADC_ADMUX_INIT
#error "Not defined ADC_ADMUX_INIT"
#endif

// ������ ������� ��� �������������
extern __flash uint8_t const adcChannel[ADC_CHANNEL_COUNT];

// ��������� ��������� ��������
extern  uint16_t adcValue[ADC_CHANNEL_COUNT];

void adcInit();
void adcStart();
void adcComplete();

#endif
