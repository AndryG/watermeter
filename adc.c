#include "adc.h"

// ������ ������� ��� �������������
__flash uint8_t const adcChannel[ADC_CHANNEL_COUNT] = ADC_CHANNEL_LIST;

// ��������� ��������� ��������
uint16_t adcValue[sizeof adcChannel];

// ������� �������� ������
static uint8_t chCounter = 0xFF;

/* ������������� ADC  */
void adcInit(){
//  ADMUX = ADC_ADMUX_INIT | adcChanel[0];
  ADCSRA = (1<<ADEN)|(0<<ADSC)|(0<<ADFR)|(1<<ADIF)|(0<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(0<<ADPS0);
}

/* ��������� ����� �������������� */
void adcStart(){
  if(0xFF == chCounter){//����� ����� ��������������
    chCounter = sizeof adcChannel - 1;
  }else if(ADCSRA & _BV(ADIF)){//�������������� ���������
    adcValue[chCounter] = ADC;
    if(0xFF == --chCounter){ // ����������� �����
      qtTask(adcComplete, 0);
      return;
    }
  }else{
    qtTask(adcStart, 0);
    return; // ������� ��������� ��������������
  }
  //������ �������������� �� ������ �� ��������
  ADMUX = ADC_ADMUX_INIT | adcChannel[chCounter];
  ADCSRA |= bv(ADSC) | bv(ADIF); // ������ �������������� c �������� �����
  qtTask(adcStart, 0);
}
