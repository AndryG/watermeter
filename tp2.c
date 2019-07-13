#include "tp2.h"

static inline tick_t tp2_mask(uint8_t Tp2){
  return (1 << Tp2) - 1;
}

bool tp2_test(tick_t ticks, uint8_t Tp2){
  return (ticks & tp2_mask(Tp2)) == 0;
}

tick_t tp2_calcDelay(tick_t ticks, uint8_t Tp2){
  tick_t n = 0;
  Tp2 = tp2_mask(Tp2);
  while(1){
    ticks += 1; n+= 1;
    if((ticks & Tp2) == 0){
      return n;
    }
  }
}

tick_t tp2_calcDelay2(tick_t ticks, uint8_t Tp2A, uint8_t Tp2B){
  tick_t n = 0;
  Tp2A = tp2_mask(Tp2A);
  Tp2B = tp2_mask(Tp2B);
  while(1){
    ticks += 1; n+= 1;
    if((ticks & Tp2A) == 0){
      return n;
    }
    if((ticks & Tp2B) == 0){
      return n;
    }
  }
}

/*
tp2_ticks_t tp2_calcDelayArr(tp2_ticks_t ticks, uint8_t Tp2[], uint8_t cnt){
  tp2_ticks_t n = 0;
  while(1){
    ticks += 1; n+= 1;
    for(uint8_t i = 0; i < cnt; i += 1){
      if(tp2_test(ticks, Tp2[i])){
        return n;
      }
    }
  }
}
*/