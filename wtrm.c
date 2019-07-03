#include "wtrm.h"

/** Флаги прерываний */
volatile u8 intf;

/** Флаги состояний */
u8 flag;
#define F_TICK	0x01	// 125ms

#define TICK_BY_SEC     0x04
#define TIMER2_COMP     32768ul / 1024u / TICK_BY_SEC

// (*(volatile u8*)(&(port) - 2))

chanel_t ch[2];

u8 chMask; // активные каналы


void main() __attribute__((noreturn));
void init();




void main(){

  init();
  while(1){
    // прерывание таймера - обрабатываем флаг по месту без обработчика
    if(TIFR & TOV2 << 1){
      TIFR |= TOV2 << 1;
      qtDecrementDelay();
    }
    qtDispatch();
  }

}

/**
 Начальная настройка. Запускается единожды при старте программы
 */
void init(){

  nRF24L01 nrf;

//  gpio_pin ce = {&PORTB, 1};

  //nrf.ce = ce;

  nRF24L01_begin(&nrf);

	qtInit();
  t2_init(T2_4Hz);

	//TODO отключить компаратор
	DDRB |= _BV(2); // snrLight
	DDRC |= _BV(3); // snrSensor

	/* дебажные штуки */
	DDRD |= 0xF0;    // debug output PORTD_Bit4-7

	qtInit();

	TCCR1B = (0 << WGM13) | (1 << ICNC1) | (1 << ICES1) | (0 << WGM12) | (0 << CS12) | (1 << CS11) | (0 << CS10); // таймер
	TIMSK |= _BV(OCIE1A);   // qt timer


	PORTB |= 0xF0;   // buttons
	//qtTask(kbdProcess, 0);
/*
#if MODE == M_ADC
	qtTask(adcInit, 0);
	snrLight(true);
	snrSensor(true);
	qtTask(adcStart, 2);
#elif MODE == M_SNR
	qtTask(snrStart, 1);
#elif MODE == M_MIX
	qtTask(adcInit, 0);
	qtTask(snrStart, 1);
#else
#error "Unknown MODE value"
#endif
*/
	uart_init(UART_BAUD_RATE); // UART
}

#define T2_1Hz  5
#define T2_2Hz  4
#define T2_4Hz  3
/* Включение/переключение асинхронного режима T2
 * Делитель 3/4/5 32/64/128 4/2/1 Гц
 */
void t2_init(u8 clockSelect){
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    ASSR = (AS2 << 1);
    TCCR2 = clockSelect & 0x07;
    while(t2_isBusy()){
      // wait
    }
	  TIFR |= bv(1 << OCF2);
    TIMSK |= bv(1<<TOIE2);
  }
}

/* признак занятости T2 */
bool t2_isBusy(){
  return (ASSR & (TCN2UB << 1 | OCR2UB << 1 | TCR2UB << 1)) == (TCN2UB << 1 | OCR2UB << 1 | TCR2UB << 1);
}

/** Отсчет системных тиков */
ISR(TIMER2_COMP_vect){
	flag |= F_TICK;
}


  //TIMSK = (1 << OCIE2) | (1 << TOIE2) | (1 << TICIE1) | (1 << OCIE1A) | (1 << OCIE1B) | (1 << TOIE1) | (1 << TOIE0)

  // таймер2 для отсчета планировщика
	//TIMSK  = (1 << OCIE2) | (0 << TOIE2) | (0 << TICIE1) | (0 << OCIE1A) | (0 << OCIE1B) | (0 << TOIE1) | (0 << TOIE0);
  //ASSR  = (1 << AS2) | (0 << TCN2UB) | (0 << OCR2UB) | (0 << TCR2UB);
  //TCCR2 = (0 << FOC2) | (0 << COM21) | (0 << COM20) | (0 << WGM21) | (0 << WGM20) | (1 << CS22) | (1 << CS21) | (1 << CS20);
  //TCNT2 = 0x00;
  //OCR2  = 0x00;



void sensorProcessing(chanel_t *ch){

}

/* Обработка датчиков: активные каналы, запуск считывания каналов
 */
void processSensorStep0(){
  snrSwapValues();
  u8 chMask = 0x03;
  snrBegin(chMask, ch[0].ledPower, ch[1].ledPower);
  // запуска таймера
}

void processSensorStep1(){
  // анализ цифровых результатов
  snrReadD(chMask);
  sensorProcessing(&channels[0]);
  snrAutomat(&channels[1]);
}