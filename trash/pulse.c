volatile uint8_t flag;
#define F_TICK        0x01 // системный тик
#define F_SNR_READY   0x02 // таймер "данные сенсора  готовы"
#define F_DSK_IMPULS  0x20 // "новый импульс с диска"

volatile uint8_t snrValue;

#define snrLight(x)   PORTB_Bit2 = (x);

#define snrSensor(x)  PORTC_Bit3 = (x);

void init();
void snrStart();
void snrReady();
void kbdProcess();

int main(){
  init();
  sei();
  while(1){
    if(flag & F_TICK){
      flag &= ~F_TICK;
      PORTD ^= _BV(4);
      qtDecrementDelay2(1);
    }
    if(flag & F_SNR_READY){
      flag &= ~F_SNR_READY;
      uint8_t impuls = dskAutomat(snrValue);
      if(impuls){
        PORTD |= _BV(7);
        _delay_ms(50);
        PORTD &= _BV(7);
      }
#if MODE == M_SNR
      snrReady();
#elif MODE == M_MIX
      adcStart();
#else
  #error "Unknown MODE"
#endif
    }
    qtDispatch();
  }
}

void init(){
  TIMSK  = (0<<OCIE2)|(0<<TOIE2)|(0<<TICIE1)|(0<<OCIE1A)|(0<<OCIE1B)|(0<<TOIE1)|(1<<TOIE0);
  //TODO отключить компаратор
  DDRB_Bit2 = 1; // snrLight
  DDRC_Bit3 = 1; // snrSensor

  /* дебажные штуки */

  DDRD |= 0xF0;    // debug output PORTD_Bit4-7

  qtInit();
  TCCR1B = (0<<WGM13)|(1<<ICNC1)|(1<<ICES1)|(0<<WGM12)|(0<<CS12)|(1<<CS11)|(0<<CS10); // таймер
  TIMSK |= _BV(OCIE1A);   // qt timer


  PORTB |= 0xF0;   // buttons
  qtTask(kbdProcess, 0);

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

  uart_init(UART_BAUD_RATE); // UART

  pwmPower(0xff);

}

// запустить цифровое считывание датчика
void snrStart(){
  qtTask(snrStart, 1);
  // свет, датчик, таймер
  snrLight(true);
  snrSensor(true);
  TCNT0 = 0xC2; // 496us
  TCCR0 = (0<<CS12)|(1<<CS11)|(1<<CS10);
}

#pragma vector=TIMER0_OVF_vect
__interrupt void TIMER0_OVF(){
  // показания, свет, сенсор, таймер, flag
  snrValue = PINC & 0x07;
#if MODE == M_SNR
  snrLight(false);
  snrSensor(false);
#endif
  TCCR0 = 0; // останов таймера
  flag |= F_SNR_READY;
}

/*Отправить в COM порт цифровые значения */
void snrReady(){
  char buf[5];
  uart_putc(snrValue & 0x01 ? '1' : '0');
  uart_putc(' ');
  uart_putc(snrValue & 0x02 ? '1' : '0');
  uart_putc(' ');
  uart_putc(snrValue & 0x04 ? '1' : '0');
  uart_putc(' ');/**/
  uart_puts(itoa16(snrValue, buf));
//  uart_puts(itoa16(dskSameCnt, buf));
#if MODE == M_SNR
  uart_putc('\r');
#endif
}

void kbdProcess(){ // обработка нажатия кнопок
  kbd_process(__swap_nibbles(PINB));
  qtTask(kbdProcess, 5);
  if(kbd_state & KBD_0){ pwmMinus(10);}
  if(kbd_state & KBD_1){ pwmPlus(10);}
  if(kbd_state & KBD_2){ pwmPower(pwmState() ? 0 : 1);}
  if(kbd_state & KBD_3){  }
  kbd_state = 0;
}

/* Передача в COM порт ADC значений */
void adcComplete(){
#if MODE == M_ADC
  qtTask(adcStart, 2);
#elif MODE == M_MIX
 // snrLight(false);
 // snrSensor(false);
#endif
  char buf[7];
  for(u8 i = 0; i < sizeof(adcChannel); i += 1){
    uart_puts(itoa16(adcValue[i], buf));
    uart_putc(' ');
  }
//
//  uint8_t i = sizeof adcChannel - 1;
//  do{
//    uart_puts(itoa16(adcValue[i], buf));
//    uart_putc(' ');
//  }while(i-- > 0);
#if MODE == M_MIX
  snrReady();
#else
  uart_putc('0');
  uart_putc(' ');
#endif
/*  i = sizeof adcChannel - 1;
  do{
    uart_puts(itoa16(1023 - adcValue[i], buf));
    uart_putc(' ');
  }while(i-- > 0);
/**/
  uart_putc('\r');
}

/** Отсчет системных тиков */
#pragma vector=TIMER1_COMPA_vect
__interrupt void TIMER1_COMPA(){

//  OCR1A += 0x270F; // 10ms
  OCR1A += 0x1387; // 5ms
// OCR1A += 0x0BB7; // 3ms
  //OCR1A += 0x07CF; // 2ms
  //OCR1A += 0x03E7; // 1ms
  flag |= F_TICK;
}
//----------------------------------------------------
/*
int main2(){
  qtInit();
  // dbg
  DDRD_Bit4 = 1;
  // ADC
  qtTask(adcInit, 0);
//  qtTask(adcStart, 0);
  DDRC = 0x00;
  // buttons
  PORTB |= 0xF0;
  // debug output PORTB_Bit4-7
  DDRB |= 0x0F;
  qtTask(kbdProcess, 0);
  // UART
  uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );
  // ticks
  TCCR0 = (0<<CS02) | (1 << CS01) | (1 << CS00); // x/64
  // sensor
  TCCR1A = (0<<WGM11)|(0<<WGM10)|(0<<COM1A1)|(0<<COM1A0)|(0<<COM1B1)|(0<<COM1B0)|(0<<FOC1A)|(0<<FOC1B);
  TIMSK  = (0<<OCIE2)|(0<<TOIE2)|(1<<TICIE1)|(0<<OCIE1A)|(0<<OCIE1B)|(1<<TOIE1)|(1<<TOIE0);
  // PWM
  pwmState(0);
  pwmPower(180);
  snrInit();
  __enable_interrupt();
  while(1){
    if(flag & F_TICK){
      flag &= ~F_TICK;
      qtDecrementDelay();
    }
    if(flag & F_SNR_READY){
      flag &= ~F_SNR_READY;
      qtTask(snrReady, 0);
    }
    qtDispatch();
  }
}
volatile uint8_t snrVal;
volatile uint16_t tcntVal;

void snrInit(){
  PORTB_Bit0 = 0;
  DDRB_Bit2  = 1;
  DDRB_Bit1  = 1;
  qtTask(snrStart2, 0);
}
/*
Запуск считывания датчика:
 включить свет, запустить таймер, включить заряд, конф. вход сенсора
* /
void snrStart2(){
  qtTask(snrStart2, 3);
 // PORTB_Bit2 = 1; // свет
  delay_us(10);
  // флаги свет заряд вход таймер
  PORTB_Bit1 = 1; // заряд
  if((kbd_ss & KBD_3) == 0){
    delay_us(2000);
  }
  PORTB_Bit2 = 1; // свет
  delay_us(2000);
  PORTB_Bit1 = 0;
  PORTB_Bit2 = 0;
}
void snrStart3(){
  // флаги свет заряд вход таймер
  PORTB_Bit1 = 1; // заряд
  __no_operation();
  __no_operation();
  __no_operation();
  flag      &= ~(F_SNR_TMR | F_SNR_CPT | F_SNR_OVF); // флаги
  DDRB_Bit0  = 0; // сенсор вход
  PORTB_Bit2 = 1; // свет
  TCNT1      = 0;
  TCCR1B     = (0<<WGM13)|(1<<ICNC1)|(1<<ICES1)|(0<<WGM12)|(0<<CS12)|(0<<CS11)|(1<<CS10); // таймер
  tcntVal    = 0;
  qtTask(snrStart, 25);
}


void snrTime(){
  // показания, свет, таймер, откл.заряд, разряд
//  snrVal     = PINB_Bit0; // показания
  snrVal    += 1; // показания
  PORTB_Bit2 = 0;         // свет
  TCCR1B    &= 0x07;      // таймер
  PORTB_Bit1 = 0;         //откл.заряд
  DDRB_Bit0  = 1;         // разряд
  flag      |= F_SNR_TMR; // флаг
}

void snrReady(){
  char buf[7];
  uart_puts(itoa16(tcntVal, buf));
  uart_putc(' ');
  uart_puts(itoa16(snrVal, buf));
  uart_putc('\r');
}
/*
void t1Start();

#define M_CHARGE       0x38
#define M_INPUT        0x07

volatile uint8_t inp; // считанные значения с датчиков

// запуск считывания датчиков
void tReadStart(){
  PORTD_Bit4 = 1;
  t1Start();
}

void tInputComplete(){
  PORTD_Bit5 = inp & 0x01;
  PORTD_Bit6 = inp & 0x02;
  PORTD_Bit7 = inp & 0x04;
  qtTask(tReadStart, 1);
  PORTD_Bit4 = 0;
}

void t1Start(){
  TCNT1  = 0;
  OCR1A  = 0x289F; // 1300us
  TCCR1B = (0<<WGM13)|(0<<ICNC1)|(1<<ICES1)|(0<<WGM12)|(0<<CS12)|(0<<CS11)|(1<<CS10);
  // заряд
  PORTC  = M_CHARGE;
  DDRC   = M_CHARGE;
}

void t1Stop(){
  TCCR1B &= 0x07;
  // разряд
  PORTC  = 0;
  DDRC   = M_INPUT;
}* /

#pragma vector=TIMER1_CAPT_vect
__interrupt void TIMER1_CAPT(){
  tcntVal    = TCNT1;
  flag |= F_SNR_CPT;
  snrTime();
}

#pragma  vector=TIMER1_OVF_vect
__interrupt void TIMER1_OVF(){
  flag |= F_SNR_OVF;
  snrTime();
}
/*
#pragma vector=TIMER1_COMPA_vect
__interrupt void TIMER1_COMPA(){
  inp = PINC;
  t1Stop();
  qtTask(tInputComplete, 0);
}

  int main3(){
  DDRD_Bit7 = 1;
  PORTD_Bit7 = 1;
  delay_ms(300);
  while(1){
    PORTD_Bit7 = !!PINB_Bit1;
  }
  qtInit();
  qtTask(tReadStart, 0);
  //
  DDRD  = 0xF0; // светики
  // Timer
  TCCR0  = (0<<CS02)|(1<<CS01)|(1<<CS00);
  TCCR1A = (0<<WGM11)|(0<<WGM10)|(0<<COM1A1)|(0<<COM1A0)|(0<<COM1B1)|(0<<COM1B0)|(0<<FOC1A)|(0<<FOC1B);
  TIMSK  = (0<<OCIE2)|(0<<TOIE2)|(1<<TICIE1)|(1<<OCIE1A)|(0<<OCIE1B)|(0<<TOIE1)|(1<<TOIE0);
  __enable_interrupt();
  while(1){
    if(timer){
      timer = 0;
      qtDecrementDelay();
    }
    qtDispatch();
  }
}   */




void t2_initOcrMode(u8 ticks){
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    ASSR  = bv(AS2);
    TCNT2 = OCR2 = 0;
    TCCR2 = 0x05; // div 128 макс период 1s
    t2_setOCRandWait(ticks);
    TIFR |= bv(OCF2);
    TIMSK|= bv(OCIE2);
    TIMSK|= bv(TOIE2);
  }
}

void t2_setOCRandWait(u8 ticks){
  //  OCR2 += 32768uL / 128ul / (1 << TP2_BY_SEC) * ticks - 1;
  //OCR2 += 0x80;
  return;
  while(t2_isBusy()){
    //while
  }
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





// static const u8 route[7][4] PROGMEM = {
// //    0     1     2     3      вх. символ
//   {0x00, 0x01, 0x02, 0x00}, // 0 начальное состояние
//   {0x00, 0x02, 0x04, 0x03}, // 1 s0-вход (первая 1)
//   {0x00, 0x02, 0x04, 0x03}, // 2 s0-повтор ( послед. 1)
//   {0x00, 0x02, 0x04, 0x03}, // 3 s0-промежуточный (послед. 3)
//   {0x00, 0x01, 0x05, 0x06}, // 4 s1-вход (первая 2)
//   {0x00, 0x01, 0x05, 0x06}, // 5 s1-повтор ( послед. 2)
//   {0x00, 0x01, 0x05, 0x06}, // 6 s1-промежуточный (послед. 3)
// };
//
// void channelProcess(ch_t *ch){
//   if(0 != ch->status){
//     return;
//   }
//   ch->state = pgm_read_byte(&route[ch->state][ch->sensor]);
//   switch(ch->state){
//     case 1:
//     case 4:
//       ch->value += 1;
//       //break
//     case 0:
//       ch->ss = 2;
//       ch->ds = 2;
//       break;
//     case 2:
//     case 5:
//       if(ch->ss){
//         ch->ss += 1;
//       }
//       //break;
//     case 3:
//     case 6:
//       if(ch->ds){
//         ch->ds += 1;
//       }
//       break;
//   }
// }

// Не стал делать отдельный заголовочный файл так-как это неотъемлемая часть проекта.
// Попробую как оно с меньшим к-во файлов

// предыдущий символ
//uint8_t dskPrevCh;

// счетчик повторов последнего вх. символа
//uint8_t dskSameCnt;

//static uint8_t state;

/*
  011   101   110   011
   A 001 B 100 C 010 A
   3  1  5  4  6  2  3
* /
static uint8_t route[7][8] = {
//      0     1     2     3     4     5     6     7 вх. символ
    {0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x03, 0x00}, // 0 z
    {0x00, 0x01, 0x01, 0x01, 0x04, 0x82, 0x01, 0x04}, // 1 A 3
    {0x00, 0x02, 0x05, 0x02, 0x02, 0x02, 0x83, 0x05}, // 2 B 5
    {0x00, 0x06, 0x03, 0x81, 0x03, 0x03, 0x03, 0x06}, // 3 C 6
    {0x00, 0x04, 0x04, 0x41, 0x04, 0x82, 0x04, 0x04}, // 4 d 4
    {0x00, 0x05, 0x05, 0x05, 0x05, 0x42, 0x83, 0x05}, // 5 e 2
    {0x00, 0x06, 0x06, 0x81, 0x06, 0x06, 0x43, 0x06}, // 6 f 1
};

/*static uint8_t revers[3][3] => {
  {}, // 0
  {}, // 1
  {}, // 2
};*/

/*
static uint8_t route[8][8] = {
//   0  1  2  3  4  5  6  вх. символ
    {0, 1, 2, 3, 4, 5, 6, 7},  // 0 - 000 обрыв
    {0, 1, 1, 1, 1, 5, 1, 7},  // 1 - 001 a-b
    {0, 2, 2, 3, 2, 2, 2, 7},  // 2 - 010 c-a
    {0, 1, 3, 3, 3, 5, 3, 7},  // 3 - 011 A
    {0, 4, 4, 4, 4, 4, 6, 7},  // 4 - 100 b-c
    {0, 5, 5, 5, 4, 5, 6, 7},  // 5 - 101 B
    {0, 6, 2, 3, 6, 6, 6, 7},  // 6 - 110 C
    {0, 1, 2, 3, 4, 5, 6, 7},  // 7 - 111 засветка
};

static bool res[8] = {0, 0, 0, 1, 0, 1, 1, 0};

/* автомат обработки данных датчиков с диска
 @param три бита состояния счетчика
 @return к-во принятых импульсов 0 - диск на прежнем состоянии
* /
uint8_t dskAutomat(uint8_t ch){
  uint8_t prev = state;
  state = route[state][ch];
  return prev != state;
}
/*
uint8_t dskAutomat2(uint8_t ch){
  uint8_t prev = state;
  state = route[state][ch];
  // считаем повторы вх. симв
  if(dskPrevCh == ch){
    dskSameCnt += 1;
  }else{
    dskPrevCh  = ch;
    dskSameCnt = 0;
  }
  // был новый символ или нет?
  if(prev != state){
    return res[state];
  }else{
    return false;
  }
}/**/
