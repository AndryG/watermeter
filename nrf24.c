#include CONFIG_FILE

/*#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/delay.h>*/
#include <string.h>
#include "nrf24.lib/src/nrf24l01-mnemonics.h"
#include "nrf24.lib/src/nrf24l01.h"

void setup_timer(void);
nRF24L01 *setup_rf(void);

volatile bool int_timer = false;
volatile bool rf_interrupt = false;

nRF24L01 *setup_rf(void);
void process_message(char *message);
void prepare_led_pin(void);
void  set_led_high(void);
void  set_led_low(void);
int main_rx(void);
int main_tx(void);



void main(void){
  PORTD_Bit7 = 1;
  prepare_led_pin();
  if(PIND_Bit7){
    main_tx();
  }else{
    main_rx();
  }
}

int main_rx(void) {
    uint8_t address[5] = { 0x01, 0x01, 0x01, 0x01, 0x01 };
    prepare_led_pin();
    __enable_interrupt();
    nRF24L01 *rf = setup_rf();
    nRF24L01_listen(rf, 0, address);
    uint8_t addr[5];
    nRF24L01_read_register(rf, CONFIG, addr, 1);

    while (true) {
        if (rf_interrupt) {
            rf_interrupt = false;
            while (nRF24L01_data_received(rf)) {
                nRF24L01Message msg;
                nRF24L01_read_received_data(rf, &msg);
                process_message((char *)msg.data);
            }

            nRF24L01_listen(rf, 0, address);
        }
    }
}

int main_tx(void) {
    uint8_t to_address[5] = { 0x01, 0x01, 0x01, 0x01, 0x01 };
    bool on = false;
    __enable_interrupt();
    nRF24L01 *rf = setup_rf();
    setup_timer();

    while (true) {
        if (rf_interrupt) {
            rf_interrupt = false;
            int success = nRF24L01_transmit_success(rf);
            if (success != 0)
                nRF24L01_flush_transmit_message(rf);
        }

        if (int_timer) {
            int_timer = false;
            on = !on;
            nRF24L01Message msg;
            if (on){
              memcpy(msg.data, "ON", 3);
              set_led_low();
            }else{
              memcpy(msg.data, "OFF", 4);
              set_led_high();
            }
            msg.length = strlen((char *)msg.data) + 1;
            nRF24L01_transmit(rf, to_address, &msg);
        }
    }
}

nRF24L01 *setup_rf(void) {
    nRF24L01 *rf = nRF24L01_init();
    rf->ss.port = &PORTB;
    rf->ss.pin = PB2;
    rf->ce.port = &PORTB;
    rf->ce.pin = PB1;
    rf->sck.port = &PORTB;
    rf->sck.pin = PB5;
    rf->mosi.port = &PORTB;
    rf->mosi.pin = PB3;
    rf->miso.port = &PORTB;
    rf->miso.pin = PB4;
    // interrupt on falling edge of INT0 (PD2)
    EICRA |= _BV(ISC01);
    EIMSK |= _BV(INT0);
    nRF24L01_begin(rf);
    return rf;
}

void process_message(char *message) {
    if (strcmp(message, "ON") == 0)
        set_led_high();
    else if (strcmp(message, "OFF") == 0)
        set_led_low();
}

inline void prepare_led_pin(void) {
    DDRB |= _BV(PB0);
    PORTB &= ~_BV(PB0);
}

inline void set_led_high(void) {
    PORTB |= _BV(PB0);
}

inline void set_led_low(void) {
    PORTB &= ~_BV(PB0);
}

// setup timer to trigger interrupt every second when at 1MHz
void setup_timer(void) {
    TCCR1B |= _BV(WGM12);
    TIMSK1 |= _BV(OCIE1A);
    OCR1A = 0x1E83;//0x3D08;
    TCCR1B |= _BV(CS10) | _BV(CS11);
}

// each one second interrupt
#pragma vector=TIMER1_COMPA_vect
__interrupt void TIMER1_COMPA(void){
   int_timer = true;
}

// nRF24L01 interrupt
#pragma vector=INT0_vect
__interrupt void tINT0(void) {
    rf_interrupt = true;
}

