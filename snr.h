#ifndef SNR_H
#define SNR_H

typedef struct snrVal_t {
  u8 state;
} snrVal_t;

extern snrVal_t (*snrVal)[2];

extern void snrBegin(u8 chMask, u8 ledPower0, u8 ledPower1);

extern void snrReadD(u8 chMask);

extern void snrOff();

extern void snrSwapValues();

#endif