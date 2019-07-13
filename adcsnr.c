

//tick_t ticks;

void main() __attribute__((noreturn));
void init();

void adcComplete(){
  qtTask(adcStart, 1 << (TP2_BY_SEC - 4)); // 2^4 раз в секунду
  char buf[7];
  for(u8 i = 0; i < sizeof(adcChannel); i += 1){
    uart_puts(itoa16(adcValue[i], buf));
    uart_putc(' ');
  }
  uart_putc('\r');
}

void main(){
  init();
  snr_begin0(3);
  qtTask(adcStart, 0);
  sei();
  while(1){
    if(isr & bv(isr_TICK)){
      isr &= ~bv(isr_TICK);
      t2_setOCRandWait(1);
      qtDecrementDelay();
    }
    qtDispatch2();
  }
}

/**
 Начальная настройка. Запускается единожды при старте программы
 */
void init(){
	qtInit();

	DDRD |= bv(PD2);    // debug line
  qtTask(&blink, (1 << (TP2_BY_SEC - 1)));

	uart_init(UART_BAUD_RATE);
  t2_initOcrMode(1);
}
