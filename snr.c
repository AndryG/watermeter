#include "snr.h"

static snrVal_t snrvAllValues[2][2];

snrVal_t (*snrVal)[2]            = &snrvAllValues[0]; // текущие для обработки значения

static snrVal_t (*snrNextVal)[2] = &snrvAllValues[1]; // считываемые значения, которые будут обработаны в след. цикле


#define SNR_DDR_LED  (*(volatile u8*)(&(SNR_PORT_LED) - 1))
#define SNR_DDR_OUT  (*(volatile u8*)(&(SNR_PORT_OUT) - 1))
#define SNR_DDR_IN   (*(volatile u8*)(&(SNR_PORT_IN) - 1))
#define SNR_PIN_IN   (*(volatile u8*)(&(SNR_PORT_IN) - 2))


void snrSwapValues(){
  void *tmp;
  tmp = snrVal;
  snrVal = snrNextVal;
  snrNextVal = tmp;
}

/* Начало считывания датчиков
 */
void snrBegin(u8 chMask, u8 ledPower0, u8 ledPower1){
  if(chMask & 0x01){
    SNR_DDR_OUT  |= bv(SNR_CH0_OUT);
    SNR_PORT_OUT |= bv(SNR_CH0_OUT);

    SNR_DDR_IN   &= ~(3 << SNR_CH0_IN);
    SNR_PORT_IN  &= ~(3 << SNR_CH0_IN);

    SNR_DDR_LED  |=  ~(3 << SNR_CH0_LED);
    SNR_PORT_LED &=  ~(3 << SNR_CH0_LED);
    SNR_PORT_LED |=  ledPower0 << SNR_CH0_LED;
  }
  if(chMask & 0x02){
    SNR_DDR_OUT  |= bv(SNR_CH1_OUT);
    SNR_PORT_OUT |= bv(SNR_CH1_OUT);

    SNR_DDR_IN   &= ~(3 << SNR_CH1_IN);
    SNR_PORT_IN  &= ~(3 << SNR_CH1_IN);

    SNR_DDR_LED  |=  ~(3 << SNR_CH1_LED);
    SNR_PORT_LED &=  ~(3 << SNR_CH1_LED);
    SNR_PORT_LED |=  ledPower1 << SNR_CH0_LED;
  }
}

/* Чтение цифровых значений с датчиков
 * @param u8 маска канало для чтения 1, 2, 3
 */
void snrReadD(u8 chMask){
  if(chMask & 0x01){
    snrNextVal[0]->state = (SNR_PORT_IN & (3 << SNR_CH0_IN)) >> SNR_CH0_IN;
  }
  if(chMask & 0x02){
    snrNextVal[1]->state = (SNR_PORT_IN & (3 << SNR_CH1_IN)) >> SNR_CH1_IN;
  }
}

/* Отключение считывателя. Перевод в режим мин. потребления сразу оба канала.
 * Даже если канал не используется, все равно линии нужно перевести в режим мин. потребления
 */
void snrOff(){
  SNR_DDR_OUT  &= ~(bv(SNR_CH0_OUT) | bv(SNR_CH1_OUT));
  SNR_PORT_OUT &= ~(bv(SNR_CH0_OUT) | bv(SNR_CH1_OUT));

  SNR_DDR_IN   &= ~(3 << SNR_CH0_IN | 3 << SNR_CH1_IN);
  SNR_PORT_IN  &= ~(3 << SNR_CH0_IN | 3 << SNR_CH1_IN);

  SNR_DDR_LED  &=  ~(3 << SNR_CH0_LED | 3 << SNR_CH1_LED);
  SNR_PORT_LED &=  ~(3 << SNR_CH0_LED | 3 << SNR_CH1_LED);
}