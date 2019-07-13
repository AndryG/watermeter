/*
 * m328.c
 *
 * Created: 13.07.2019 10:12:33
 * Author : user
 */

#include <avr/io.h>
#define F_CPU	8000000UL

#include <util/atomic.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>



#include "lib.h"
#include "hal.h"
#include "itoa.h"

/* "tp2" период равный 1 секунде
 * Чтобы получить к-во тиков - возведи двойку в степень: 1 << tp2_bysec
 * Для установки периода "два раза в секунду" уменьшить TP2_BY_SEC на единицу, если он сам не равен единице
 */
#define TP2_BY_SEC    3 // 3 - каждый 8 тик - секунда


/** Флаги прерываний */
volatile u8 isr;
#define isr_TICK  0
#define isr_T0    1
#define isr_NRF   2

void main() __attribute__((noreturn));
void init();
void t2_initOcrMode(u8 ticks);
void t2_setOCRandWait(u8 ticks);
void t2_initOvfMode(u8 ticks);
void t2_setOVFandWait(u8 ticks);

void blink();
void tc0_start();
void sensorTask();

void main(){
  init();
  DDRB = 0xff;
  sei();
  while(1){
//PORTB ^= 4;

/*      continue;
    if(isr & bv(isr_T0)){
    //  qtTask(&sensorTask, 0); // Задача отвечает за сброс флага после обработки
    }
    if(isr & bv(isr_TICK)){
      isr &= ~bv(isr_TICK);
      t2_setOCRandWait(1);
      ticks += 1;
      PORTB ^= 1;
      qtDecrementDelay();
    }
    if(!qtDispatch2()){
      if(0 != TCCR0){ // работает таймер

      }
      // задач нет! Можно попробовать поспать
      //    sleep_mode();
      //set_sleep_mode(SLEEP_MODE_PWR_SAVE);
    }    */
  }
}

/**
 Начальная настройка. Запускается единожды при старте программы
 */
void init(){
	DDRD |= bv(PD2);    // debug line
  t2_initOvfMode(0);
//  t2_initOcrMode(0);
}

/* признак занятости T2 */
inline bool t2_isBusy(){
  return (ASSR & (bv(TCR2BUB) | bv(TCR2AUB) | bv(OCR2BUB) | bv(OCR2AUB) | bv(TCN2UB))) != 0;
}

void t2_setOCRandWait(u8 ticks){
  OCR2A += 32768uL / 128ul / (1 << TP2_BY_SEC) * ticks - 1;
//  OCR2A += 0x80;
  //return;
  while(t2_isBusy()){
    //while
  }
}

void t2_setOVFandWait(u8 ticks){
  TCNT2 = 0xff + 1 - 32768uL / 128ul / (1 << TP2_BY_SEC) * ticks;// + 1;
  while(t2_isBusy()){
    //while
  }
}

ISR(TIMER2_COMPA_vect){
  isr |= bv(isr_TICK);
  //t2_setOCRandWait(4);
  PORTB ^= 1;
}

ISR(TIMER2_OVF_vect){
  isr |= bv(isr_TICK);
  t2_setOVFandWait(6);
  PORTB ^= 2;
}

void t2_initOcrMode(u8 ticks){
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    ASSR  = bv(AS2);
    TCNT2 = OCR2A = 0;
    TCCR2B = 0x05; // div 128 макс период 1s
    t2_setOCRandWait(ticks);
    TIFR2 |= bv(OCF2A);
    TIMSK2 |= bv(OCIE2A);
    TIMSK2 |= bv(TOIE2);
  }
}

void t2_initOvfMode(u8 ticks){
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    ASSR  = bv(AS2);
    TCCR2B = 0x05; // div 128 макс период 1s
    t2_setOVFandWait(ticks);
    TIFR2 |= bv(TOV2);
    TIMSK2|= bv(TOIE2);
  }
}

