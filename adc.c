#include "adc.h"

// список каналов для пребразования
uint8_t const adcChannel[ADC_CHANNEL_COUNT] = ADC_CHANNEL_LIST;

// последние считанные значения
uint16_t adcValue[sizeof adcChannel];

// счетчик текущего канала
static uint8_t chCounter = 0xff;

/* Инициализация ADC  */
void adcInit(){
  chCounter = 0xff;
//  ADMUX = ADC_ADMUX_INIT | adcChanel[0];
  ADCSR = 1 << ADEN | 0 << ADSC | 0<<ADFR | 1 << ADIF | 0 << ADIE | 1<<ADPS2 | 1<<ADPS1 |  0<<ADPS0 ;
}

/* запускает серию преобразований */
void adcStart(){
  if(0xFF == chCounter){//новая серия преобразований
    chCounter = sizeof adcChannel - 1;
  }else if(ADCSRA & _BV(ADIF)){//преобразование закончено
    adcValue[chCounter] = ADC;
    if(0xFF == --chCounter){ // закончилась серия
      qtTask(adcComplete, 0);
      return;
    }
  }else{
    qtTask(adcStart, 0);
    return; // ожидаем окончания преобразования
  }
  //запуск преобразования на канале по счетчику
  ADMUX = ADC_ADMUX_INIT | adcChannel[chCounter];
  ADCSRA |= _BV(ADSC) | _BV(ADIF); // запуск преобразования c очисткой флага
  qtTask(adcStart, 0);
}
