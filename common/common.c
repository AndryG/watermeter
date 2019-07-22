#include "common.h"

volatile u8 isr;

static inline bool t2_isBusy();

void t2_initOvfMode(u8 ticks){
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    TIFR |= bv(TOV2);
    TIMSK|= bv(TOIE2);
    ASSR  = bv(AS2);
    TCCR2 = 0x05; // div 128 32768 / 256 / 1 (раз в сек) = 128  период 1s
    t2_setOvfAndWait(ticks);
  }
}

void t2_setOvfAndWait(u8 ticks){
  //TCNT2 = 0xff + 1 - 32768uL / 128ul / (1 << TP2_BY_SEC) * ticks;
  TCNT2 -= 32768uL / 128ul / (1 << TP2_BY_SEC) * (ticks - 1);
  while(t2_isBusy()){
    //while
  }
}

ISR(TIMER2_OVF_vect){
  isr |= bv(isr_TICK);
  t2_setOvfAndWait(1);
}

/* признак занятости T2 */
static inline bool t2_isBusy(){
  return (ASSR & (bv(TCN2UB) | bv(OCR2UB) | bv(TCR2UB))) != 0;
}

#define TC0_RELOAD (F_CPU / 1000000ul * 496 / 64) //todo: вынести задержку (в us) конфиг

#if TC0_RELOAD > 0x100
 #error TC0_RELOAD overflow
#endif

/* Старт отсчета "разогрева" фотодиода (0,5мс)
 */
void tc0_start(){
  TCNT0 = 0xff + 1 - TC0_RELOAD;
  SFIOR |= bv(PSR10);
  //TCCR0 = 0x02; // div 8  Делитель поменьше, чтобы уменьшить влияние предделителя или его сброс
  TCCR0 = 0x03; // div 64
  TIFR |= bv(TOV0);
  TIMSK|= bv(TOIE0);
}

ISR(TIMER0_OVF_vect){
  TCCR0 = 0x00; // останов счетчика
  isr |= bv(isr_T0);
}

static void led_off(){
  LED_PORT &= ~bv(LED_PIN);
}

void led_blink(){
  if(0 == (LED_PORT & bv(LED_PIN))){
    LED_PORT |= bv(LED_PIN);
    qtTask(&led_off, bv(TP2_SUB(2))); // 0.25сек
  }
}

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

#define SNR_DDR_LED  (*(volatile u8*)(&(SNR_PORT_LED) - 1))
#define SNR_DDR_OUT  (*(volatile u8*)(&(SNR_PORT_OUT) - 1))
#define SNR_DDR_IN   (*(volatile u8*)(&(SNR_PORT_IN) - 1))
#define SNR_PIN_IN   (*(volatile u8*)(&(SNR_PORT_IN) - 2))

void snr_begin0(u8 ledPower0){
  SNR_DDR_OUT  |= bv(SNR_CH0_OUT); // выход сенсора
  SNR_PORT_OUT |= bv(SNR_CH0_OUT);

  SNR_DDR_IN   &= ~(3 << SNR_CH0_IN);  // вход сенсора
  SNR_PORT_IN  &= ~(3 << SNR_CH0_IN);

  SNR_DDR_LED  |=  ~(3 << SNR_CH0_LED); // включить свет
  SNR_PORT_LED &=  ~(3 << SNR_CH0_LED);
  SNR_PORT_LED |=  ledPower0 << SNR_CH0_LED;
}

void snr_begin1(u8 ledPower1){
  SNR_DDR_OUT  |= bv(SNR_CH1_OUT);
  SNR_PORT_OUT |= bv(SNR_CH1_OUT);

  SNR_DDR_IN   &= ~(3 << SNR_CH1_IN);
  SNR_PORT_IN  &= ~(3 << SNR_CH1_IN);

  SNR_DDR_LED  |=  ~(3 << SNR_CH1_LED);
  SNR_PORT_LED &=  ~(3 << SNR_CH1_LED);
  SNR_PORT_LED |=  ledPower1 << SNR_CH0_LED;
}


// void snr_begin0(u8 chMask, u8 ledPower0, u8 ledPower1){
//   if(chMask & 0x01){
//     SNR_DDR_OUT  |= bv(SNR_CH0_OUT);
//     SNR_PORT_OUT |= bv(SNR_CH0_OUT);
//
//     SNR_DDR_IN   &= ~(3 << SNR_CH0_IN);
//     SNR_PORT_IN  &= ~(3 << SNR_CH0_IN);
//
//     SNR_DDR_LED  |=  ~(3 << SNR_CH0_LED);
//     SNR_PORT_LED &=  ~(3 << SNR_CH0_LED);
//     SNR_PORT_LED |=  ledPower0 << SNR_CH0_LED;
//   }
//   if(chMask & 0x02){
//     SNR_DDR_OUT  |= bv(SNR_CH1_OUT);
//     SNR_PORT_OUT |= bv(SNR_CH1_OUT);
//
//     SNR_DDR_IN   &= ~(3 << SNR_CH1_IN);
//     SNR_PORT_IN  &= ~(3 << SNR_CH1_IN);
//
//     SNR_DDR_LED  |=  ~(3 << SNR_CH1_LED);
//     SNR_PORT_LED &=  ~(3 << SNR_CH1_LED);
//     SNR_PORT_LED |=  ledPower1 << SNR_CH0_LED;
//   }
// }

u8 snr_readCh0(){
  return SNR_PORT_IN >> SNR_CH0_IN & 3;
}
u8 snr_readCh1(){
  return SNR_PORT_IN >> SNR_CH1_IN & 3;
}

void snr_off(){
  SNR_DDR_OUT  &= ~(bv(SNR_CH0_OUT) | bv(SNR_CH1_OUT));
  SNR_PORT_OUT &= ~(bv(SNR_CH0_OUT) | bv(SNR_CH1_OUT));

  SNR_DDR_IN   &= ~(3 << SNR_CH0_IN | 3 << SNR_CH1_IN);
  SNR_PORT_IN  &= ~(3 << SNR_CH0_IN | 3 << SNR_CH1_IN);

  SNR_DDR_LED  &= ~(3 << SNR_CH0_LED | 3 << SNR_CH1_LED);
  SNR_PORT_LED &= ~(3 << SNR_CH0_LED | 3 << SNR_CH1_LED);
  ADCSR         = 0;
}

// u8 snr_read(){
//   #if SNR_CH1_IN > SNR_CH0_IN
//   u8 tmp = SNR_PORT_IN >> SNR_CH0_IN;
//   ch[0].sensor = tmp & 0x03;
//   ch[1].sensor = tmp >> (SNR_CH1_IN - SNR_CH0_IN) & 0x03;
//   #else
//   u8 tmp = SNR_PORT_IN >> SNR_CH1_IN;
//   ch[1].sensor = tmp & 0x03;
//   ch[0].sensor = tmp >> (SNR_CH0_IN - SNR_CH1_IN) & 0x03;
//   #endif
// }