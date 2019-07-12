
/** Флаги прерываний */
volatile u8 isr;
#define isr_TICK  0
#define isr_T0    1
#define isr_NRF   2

tp2_ticks_t ticks;

ch_t ch[2];

void main() __attribute__((noreturn));
void init();
void t2_initOcrMode(u8 ticks);
void t2_setOCRandWait(u8 ticks);
void blink();
void tc0_start();
void sensorTask();

void dbgSend(u8 ext){
  char buf[5];
  uart_puts(itoa16(ch[0].sensor, buf));
  uart_putc(','); uart_puts(itoa16(ch[1].sensor, buf));
  uart_putc(','); uart_puts(itoa16(ext, buf));
  uart_putc('\r');
}

void main(){
  init();
  sei();
  while(1){
    if(isr & bv(isr_T0)){
      qtTask(&sensorTask, 0); // Задача отвечает за сброс флага после обработки
    }
    if(isr & bv(isr_TICK)){
      isr &= ~bv(isr_TICK);
      t2_setOCRandWait(1);
      ticks += 1;
      qtDecrementDelay();
    }
    if(!qtDispatch2()){
      if(0 != TCCR0){ // работает таймер

      }
      // задач нет! Можно попробовать поспать
      //    sleep_mode();
      //set_sleep_mode(SLEEP_MODE_PWR_SAVE);
    }
  }
}

/**
 Начальная настройка. Запускается единожды при старте программы
 */
void init(){
tc0_start();
	qtInit();

	DDRD |= bv(PD2);    // debug line
  qtTask(&blink, 0);

	uart_init(UART_BAUD_RATE);
  t2_initOcrMode(1);
}

//#define chTestTp2(ch) ((ch).status < 2 && ((ch).status = tp2_test(ticks, (ch).tp2)))
//bool chTestTp2(ch_t ch){ return (ch.status < 2 && (ch.status = tp2_test(ticks, ch.tp2)))};

bool setStatusByTp2(ch_t *ch){
  if(ch->status < 2){
    ch->status = tp2_test(ticks, ch->tp2);
    return ch->status;
  }
  return false;
}


//void chWrap(void(*pf)(ch_t[])){}

/* Задача читает сенсоры и запускает обработку результатов
 * Построена на автомате для многократного запуска
 */
void sensorTask(){
  static u8 state = 0; // состояние автомата

  while(1){ // цикл для возможности внутреннего перепрыгивания по состояниям без рекурсии
    switch(state){
      u8 hasActive = false;
      case 0: // инициализация и перескок на "начало"
        #if TP2_BY_SEC < 2
          #error "TP2_BY_SEC ожидается 2+"
        #endif
        ch[0].tp2 = TP2_BY_SEC - 2; // опрос 4 раза в секунду
        ch[1].tp2 = TP2_BY_SEC - 2;
        ch[0].ledPower = 2;
        ch[1].ledPower = 2;
        //no-break

      case 1: // начало опроса сенсоров
        if(setStatusByTp2(&ch[0])){
          snr_begin0(ch[0].ledPower);
          hasActive = true;
        }
        if(setStatusByTp2(&ch[1])){
          snr_begin1(ch[1].ledPower);
          hasActive = true;
        }
        if(hasActive){
          tc0_start();
          state = 2; // не планируем задачу. Это сделает главный цикл по флагу isr_t0
          return;
        }
        state = 9; // каналов для опроса нет. Переходим к планированию след. запуска
        break;

      case 2: // приступаем к чтению значений
        if(isr & bv(isr_T0)){
          isr &= ~bv(isr_T0);
          if(0 == ch[0].status){
            ch[0].sensor = snr_readCh0();
          }
          if(0 == ch[1].status){
            ch[1].sensor = snr_readCh1();
          }
          state = 3;
        }else{ // Oops! Мы должны в это состояние приходить с поднятым флагом!
          dbgSend(DBG_LOST_ISR);
          state = 9; // восстанавливаемся после ошибки
        }
        snr_off();
        break;

      case 3: // анализ результатов
        dbgSend(0);
        channelProcess(&ch[0]);
        channelProcess(&ch[1]);
        break;

      case 9: // планирование след. запуска
        qtTask(&sensorTask, tp2_calcDelay2(ticks, ch[0].tp2, ch[1].tp2));
        state = 1;
        return;
    }
  }
}

/* признак занятости T2 */
inline bool t2_isBusy(){
  return (ASSR & (bv(TCN2UB) | bv(OCR2UB) | bv(TCR2UB))) != 0;
}

void t2_setOCRandWait(u8 ticks){
  OCR2 += 32768uL / 128ul / (1 << TP2_BY_SEC) * ticks - 1;
  while(t2_isBusy()){
    //while
  }
}

void t2_initOcrMode(u8 ticks){
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    ASSR  = bv(AS2);
    TCNT2 = OCR2 = 0;
    TCCR2 = 0x05; // div 128 макс период 1s
    t2_setOCRandWait(ticks);
	  TIFR |= bv(OCF2);
	  TIMSK|= bv(OCIE2);
  }
}

#define TC0_RELOAD (F_CPU / 1000000ul * 496 / 8) //todo: вынести задержку (в us) конфиг

#if T0_RELOAD > 0x100
 #error T0_RELOAD overflow
#endif

/* Старт отсчета "разогрева" фотодиода (0,5мс)
 */
void tc0_start(){
  TCNT0 = 0xFF + 1 - TC0_RELOAD;
  SFIOR |= bv(PSR10);
  TCCR0 = 0x02; // div 8  Делитель поменьше, чтобы уменьшить влияние предделителя
  TIFR |= bv(TOV0);
  TIMSK|= bv(TOIE0);
}

void blink(){
  PORTD ^= bv(PD2);
  qtTask(&blink, 1 << (TP2_BY_SEC - 1));
}

ISR(INT1_vect){
  isr |= bv(isr_NRF);
}

ISR(TIMER2_COMP_vect){
  isr |= bv(isr_TICK);
}

ISR(TIMER0_OVF_vect){
  TCCR0 = 0x00; // останов счетчика
  isr |= bv(isr_T0);
}

/*
const u8 PROGMEM 67*-7ByDelim[3] = {4, 8, 4};

void processSensorStep1(){
  volatile u8 ticks = pgm_read_byte(t2_ticksByDelim + (TCCR2 & 0x03));
  // анализ цифровых результатов
  snrReadD(ticks);
//  sensorProcessing(&ch[0]);
//  snrAutomat(ch[1]);
}

/*
 * Индекс периода таймера планировщика
 * К-во тиков по индексу периода 1 << Ti
 * 0 - 0.25Hz 1 tick
 * 1 - 0.50Hz 2 tick
 * 2 - 1.00Hz 4 tick
 */

//inline u8 t2_getTi(){
  //return (TCCR2 & 0x07) - 3;
//}

/*
 * Установка таймера
 */
//inline void t2_setTi(u8 Ti){
  //TCCR2 &= ~0x07;
  //TCCR2 |= 5 - Ti;
//}


/*
#define T2_1Hz  5
#define T2_2Hz  4
#define T2_4Hz  3
/* Включение/переключение асинхронного режима T2
 * Делитель 3/4/5 32/64/128 4/2/1 Гц
 * /
void t2_initDelimMode(u8 clockSelect){
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    ASSR = (AS2 << 1);
    TCCR2 = clockSelect & 0x07;
    while(t2_isBusy()){
      // wait
    }
	  TIFR |= bv(1 << OCF2);
    TIMSK |= bv(1<<TOIE2);
  }
}*/

  //TIMSK = (1 << OCIE2) | (1 << TOIE2) | (1 << TICIE1) | (1 << OCIE1A) | (1 << OCIE1B) | (1 << TOIE1) | (1 << TOIE0)

  // таймер2 для отсчета планировщика
  //TIMSK  = (1 << OCIE2) | (0 << TOIE2) | (0 << TICIE1) | (0 << OCIE1A) | (0 << OCIE1B) | (0 << TOIE1) | (0 << TOIE0);
  //ASSR  = (1 << AS2) | (0 << TCN2UB) | (0 << OCR2UB) | (0 << TCR2UB);
  //TCCR2 = (0 << FOC2) | (0 << COM21) | (0 << COM20) | (0 << WGM21) | (0 << WGM20) | (1 << CS22) | (1 << CS21) | (1 << CS20);
  //TCNT2 = 0x00;
  //OCR2  = 0x00;


  //	TCCR1B = (0 << WGM13) | (0 << ICNC1) | (0 << ICES1) | (0 << WGM12) | (0 << CS12) | (1 << CS11) | (0 << CS10); // таймер
  //	TIMSK |= bv(OCIE1A);
