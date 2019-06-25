#include "itoa.h"

static __flash uint16_t pow10Table16[] = {10000u, 1000u, 100u, 10u, 1u};

char *itoa16(int16_t value, char  *buffer){
  char *ptr = buffer;
  if(value < 0){
    *ptr++ = '-';
    value = 0 - value; // abs()
  }
  if(value > 0){
    uint16_t __flash *p10 = pow10Table16;
    while(value < *p10){ // пропускаем большие варианты вычитаемого
      p10 += 1;
    }
    while(p10 != &pow10Table16[4]){// обработка всех вычитаемых кроме единицы
      uint8_t count = '0';
      uint16_t v10 = *p10;
      *ptr = '0';
      while(value >= v10){
        count++;
        value -= v10;
      }
      *ptr++ = count ;
      p10 += 1;
    }
  }
  *ptr++ = '0' + value; // добавляем самый младший разряд
  *ptr = 0;
  return buffer;
}