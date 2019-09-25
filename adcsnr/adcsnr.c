#include "../common/common.h"
#include "adc.h"

//tick_t ticks;
void main() __attribute__((noreturn));
void init();

void adcComplete(){
  qtTask(adcStart, 1);
  char buf[7];
  for(u8 i = 0; i < sizeof(adcChannel); i += 1){
    uart_puts(itoa16(adcValue[i], buf));
    uart_putc(' ');
  }
  uart_puts(itoa16(snr_readCh0(), buf));
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
      t2_setOvfAndWait(1);
      qtDecrementDelay();
    }
    qtDispatch2();
  }
}

void init(){
	qtInit();
  led_init();
	uart_init(UART_BAUD_RATE);
  t2_initOvfMode(1);
  adcInit();
  led_blink();
}
