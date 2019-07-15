// управление PWM вентилятором
/**
  0 - отключение (pwm отключается и выход включается в 0)
  1 - возврат на значение до отключения
  >1- установка значения
  0xff - pwm отключается и выход включается в 1
*/
void pwmPower(uint8_t power){
  if(power){
    if(1 != power){
      OCR2 = power;
    }
    DDRB_Bit3 = 1;
    if(OCR2 == 0xff){
      TCCR2 &= ~((1<<COM21)|(1<<COM20));
      PORTB_Bit3 = 1;
    }else{
      TCCR2 = (0<<FOC2)|(1<<WGM21)|(1<<WGM20)|(1<<COM21)|(0<<COM20)|(0<<CS22)|(1<<CS21)|(0<<CS20);
    }
  }else{
    TCCR2 &= ~((1<<COM21)|(1<<COM20));
    PORTB_Bit3 = 0;
  }
}

uint8_t pwmState(){
  return (TCCR2 & ((1<<COM21)|(1<<COM20))) > 0 ? OCR2 : 0;
}

void pwmPlus(uint8_t diff){
  pwmPower(OCR2 <= 0xFF - diff ? OCR2 + diff : 0xFF);
}

void pwmMinus(uint8_t diff){
  pwmPower(OCR2 = (OCR2 > diff) ? OCR2 - diff : 0);
}
