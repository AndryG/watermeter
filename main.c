#include "config.h"

volatile uint8_t flag;
#define F_TICK        0x01 // ��������� ���
#define F_SNR_READY   0x02 // ������ "������ �������  ������"
#define F_DSK_IMPULS  0x20 // "����� ������� � �����"

volatile uint8_t snrValue;

#define snrLight(x)   PORTB_Bit2 = (x);

#define snrSensor(x)  PORTC_Bit3 = (x);

void init();
void snrStart();
void snrReady();
void kbdProcess();

int main(){
  init();
//  qtTask(snrStart, 1);
  qtTask(adcStart, 2);
  __enable_interrupt();
  while(1){
    if(flag & F_TICK){
      flag &= ~F_TICK;
      PORTD_Bit4 ^= 1;
      qtDecrementDelay();
    }
    if(flag & F_SNR_READY){
      flag &= ~F_SNR_READY;
      uint8_t impuls = dskAutomat(snrValue);
      if(impuls){
        PORTD_Bit7 = 1;
        delay_ms(50);
        PORTD_Bit7 = 0;
      }
      snrReady();
    }
    qtDispatch();
  }
}

/*��������� � COM ���� �������� �������� */
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
  uart_putc('\r');
}

void init(){
  TIMSK  = (0<<OCIE2)|(0<<TOIE2)|(0<<TICIE1)|(0<<OCIE1A)|(0<<OCIE1B)|(0<<TOIE1)|(1<<TOIE0);
  //TODO ��������� ����������
  DDRB_Bit2 = 1; // snrLight
  DDRC_Bit3 = 1; // snrSensor

  /* �������� ����� */

  DDRD |= 0xF0;    // debug output PORTD_Bit4-7

  qtInit();
  TCCR1B = (0<<WGM13)|(1<<ICNC1)|(1<<ICES1)|(0<<WGM12)|(0<<CS12)|(1<<CS11)|(0<<CS10); // ������
  TIMSK |= _BV(OCIE1A);   // qt timer

  PORTB |= 0xF0;   // buttons
  qtTask(kbdProcess, 0);

  qtTask(adcInit, 0);

  uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU)); // UART

  pwmPower(0xff);

}

// ��������� �������� ���������� �������
void snrStart(){
  qtTask(snrStart, 1); // 250ms
  // ����, ������, ������
  snrLight(true);
  snrSensor(true);
  TCNT0 = 0xC2; // 496us
  TCCR0 = (0<<CS12)|(1<<CS11)|(1<<CS10);
}

#pragma vector=TIMER0_OVF_vect
__interrupt void TIMER0_OVF(){
  // ���������, ����, ������, ������, flag
  snrValue = PINC & 0x07;
  snrLight(false);
  snrSensor(false);
  TCCR0 = 0;
  flag |= F_SNR_READY;
}

void kbdProcess(){ // ��������� ������� ������
  kbd_process(__swap_nibbles(PINB));
  qtTask(kbdProcess, 5);
  if(kbd_state & KBD_0){ pwmMinus(10);}
  if(kbd_state & KBD_1){ pwmPlus(10);}
  if(kbd_state & KBD_2){ pwmPower(pwmState() ? 0 : 1);}
  if(kbd_state & KBD_3){  }
  kbd_state = 0;
}

/* �������� � COM ���� ADC �������� */
void adcComplete(){
  qtTask(adcStart, 2);
  char buf[7];
  uint8_t i = sizeof adcChannel - 1;

  do{
    uart_puts(itoa16(adcValue[i], buf));
    uart_putc(' ');
  }while(i-- > 0);
/*  i = sizeof adcChannel - 1;
  do{
    uart_puts(itoa16(1023 - adcValue[i], buf));
    uart_putc(' ');
  }while(i-- > 0);
/**/
  uart_putc('\r');
}

/** ������ ��������� ����� */
#pragma vector=TIMER1_COMPA_vect
__interrupt void TIMER1_COMPA(){
//  OCR1A += 0x1387; // 5ms
  OCR1A += 0x0BB7; // 3ms
//  OCR1A += 0x07CF; // 2ms
//  OCR1A += 0x03E7; // 1ms
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
������ ���������� �������:
 �������� ����, ��������� ������, �������� �����, ����. ���� �������
* /
void snrStart2(){
  qtTask(snrStart2, 3);
 // PORTB_Bit2 = 1; // ����
  delay_us(10);
  // ����� ���� ����� ���� ������
  PORTB_Bit1 = 1; // �����
  if((kbd_ss & KBD_3) == 0){
    delay_us(2000);
  }
  PORTB_Bit2 = 1; // ����
  delay_us(2000);
  PORTB_Bit1 = 0;
  PORTB_Bit2 = 0;
}
void snrStart3(){
  // ����� ���� ����� ���� ������
  PORTB_Bit1 = 1; // �����
  __no_operation();
  __no_operation();
  __no_operation();
  flag      &= ~(F_SNR_TMR | F_SNR_CPT | F_SNR_OVF); // �����
  DDRB_Bit0  = 0; // ������ ����
  PORTB_Bit2 = 1; // ����
  TCNT1      = 0;
  TCCR1B     = (0<<WGM13)|(1<<ICNC1)|(1<<ICES1)|(0<<WGM12)|(0<<CS12)|(0<<CS11)|(1<<CS10); // ������
  tcntVal    = 0;
  qtTask(snrStart, 25);
}


void snrTime(){
  // ���������, ����, ������, ����.�����, ������
//  snrVal     = PINB_Bit0; // ���������
  snrVal    += 1; // ���������
  PORTB_Bit2 = 0;         // ����
  TCCR1B    &= 0x07;      // ������
  PORTB_Bit1 = 0;         //����.�����
  DDRB_Bit0  = 1;         // ������
  flag      |= F_SNR_TMR; // ����
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

volatile uint8_t inp; // ��������� �������� � ��������

// ������ ���������� ��������
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
  // �����
  PORTC  = M_CHARGE;
  DDRC   = M_CHARGE;
}

void t1Stop(){
  TCCR1B &= 0x07;
  // ������
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
  DDRD  = 0xF0; // �������
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

