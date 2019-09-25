#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <util/atomic.h>

#include "../common/common.h"

tick_t ticks;

ch_t ch[2];

static const u8 route[9][4] PROGMEM;

void main() __attribute__((noreturn));
void init();
void sensorTask();
void channelProcess(ch_t *ch);

void my_exit (u16 code) __attribute__ ((naked)) __attribute__ ((section (".fini8")));
void my_exit (u16 code){
  //PORTB = code;
}

void main(){
  init();
  sei();
  uart_puts_P("start\n");
  while(1){

    if(isr & bv(isr_T0)){
      qtTask(&sensorTask, 0); // Задача отвечает за сброс флага после обработки
    }
    if(isr & bv(isr_TICK)){
      isr &= ~bv(isr_TICK);
      t2_setOvfAndWait(2);
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
    }
  }
}

/* Начальная настройка. Запускается единожды при старте программы
 */
void init(){
	qtInit();
	led_init();
	uart_init(UART_BAUD_RATE);
	t2_initOvfMode(1);
	led_blink();
}

//void my_exit (u8) __attribute__ ((naked)) __attribute__ ((section (".fini8")));


//#define chTestTp2(ch) ((ch).status < 2 && ((ch).status = tp2_test(ticks, (ch).tp2)))
//bool chTestTp2(ch_t ch){ return (ch.status < 2 && (ch.status = tp2_test(ticks, ch.tp2)))};

bool setStatusByTp2(ch_t *ch){
  if(ch->status < 2){
    ch->status = tp2_test(ticks, ch->tp2);
    return ch->status;
  }
  return false;
}

/* Задача читает сенсоры и запускает обработку результатов
 * Построена на автомате для многократного запуска
 */
void sensorTask(){
  static u8 state = 0; // состояние автомата

  while(1){ // цикл для возможности внутреннего перепрыгивания по состояниям без рекурсии
    if((isr & bv(isr_T0)) && state != 2){
      while(1){
        exit(E_SENSOR_TASK_1);
      }
    }

    switch(state){
      u8 hasActive = false;
      case 0: // инициализация и перескок на "начало"
        #if TP2_BY_SEC < 2
          #error "TP2_BY_SEC ожидается 2+"
        #endif
        ch[0].tp2 = TP2_BY_SEC - 2; // опрос 4 раза в секунду
        ch[1].tp2 = TP2_BY_SEC - 2;
        ch[0].ledPower = 3;
        ch[1].ledPower = 3;
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
          // и что тут делать ?
          state = 9; // восстанавливаемся после ошибки
        }
        snr_off();
        break;

      case 3: // анализ результатов
        channelProcess(&ch[0]);
        channelProcess(&ch[1]);
        state = 9;
        break;

      case 9: // планирование след. запуска
        qtTask(&sensorTask, tp2_calcDelay2(ticks, ch[0].tp2, ch[1].tp2));
        state = 1;
        return;
    }
  }
}

void channelProcess(ch_t *ch){
  if(0 != ch->status){
    return;
  }
  ch->state = pgm_read_byte(&route[ch->state][ch->sensor]);
  switch(ch->state){
    case 0:
    // реальное значение dss равно dss - 1. Это защита от переполнения (255 + 1 = 0 и счет останавливается)
    ch->dzs = 0;
    ch->dss = 2;
    ch->ds  = 1;
    break;

    case 1: // enter
    case 5:
    ch->value += 1;
    // проверка на учащение опросов
    // тут можно -1 перенести на другу сторону, с условием, что dss_range_min не будет 0xff
    if(ch->dss - 1 < DSS_RANGE_MIN){ // слишком мало считываний на темный сектор - учащаем опрос, уменьшая интервал
      if(ch->tp2 > 0){
        ch->tp2 -= 1;
      }
    }
    // обнуление счетчиков
    ch->dzs = 0;
    ch->dss = 2; // 1 начальное + 1 защита от переполнения
    ch->ds  = 1;
    break;

    case 2: // continued
    case 6:
    if(ch->dss){ // защита от переполнения
      ch->dss += 1;
    }
    //break;
    case 3: //mixed
    case 7:
    if(ch->ds < DS_RANGE_MAX){
      ch->ds += 1;
      }else{
      if(ch->tp2 < TP2_MAX){ // замедляем, если можно, опросы
        ch->tp2 += 1;
      }
      ch->ds = 1;
    }
    break;

    case 4: // zero
    case 8:
    if(ch->dzs < DZS_RANGE_MAX){ // порог допустимых помех
      ch->dzs += 1;
      }else{
      ch->status = CH_ZERO;
    }
    break;
  }
}

static const u8 route[9][4] PROGMEM = {
  //  0     1     2     3   вх. символ
  {0x00, 0x01, 0x05, 0x00}, // 0 начальное состояние
  {0x04, 0x02, 0x05, 0x03}, // 1 s0-вход (первая 1)
  {0x04, 0x02, 0x05, 0x03}, // 2 s0-повтор ( послед. 1)
  {0x04, 0x02, 0x05, 0x03}, // 3 s0-промежуточный (послед. 3)
  {0x04, 0x02, 0x05, 0x03}, // 4 s0-zero
  {0x08, 0x01, 0x06, 0x07}, // 5 s1-вход (первая 2)
  {0x08, 0x01, 0x06, 0x07}, // 6 s1-повтор ( послед. 2)
  {0x08, 0x01, 0x06, 0x07}, // 7 s1-промежуточный (послед. 3)
  {0x08, 0x01, 0x06, 0x07}, // 8 s1-zero
};