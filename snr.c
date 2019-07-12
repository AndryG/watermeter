#include "snr.h"

#define SNR_DDR_LED  (*(volatile u8*)(&(SNR_PORT_LED) - 1))
#define SNR_DDR_OUT  (*(volatile u8*)(&(SNR_PORT_OUT) - 1))
#define SNR_DDR_IN   (*(volatile u8*)(&(SNR_PORT_IN) - 1))
#define SNR_PIN_IN   (*(volatile u8*)(&(SNR_PORT_IN) - 2))

void snr_begin0(u8 ledPower0){
  SNR_DDR_OUT  |= bv(SNR_CH0_OUT);
  SNR_PORT_OUT |= bv(SNR_CH0_OUT);

  SNR_DDR_IN   &= ~(3 << SNR_CH0_IN);
  SNR_PORT_IN  &= ~(3 << SNR_CH0_IN);

  SNR_DDR_LED  |=  ~(3 << SNR_CH0_LED);
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