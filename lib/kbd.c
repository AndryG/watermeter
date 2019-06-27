#include <stdint.h>
#include "kbd.h"

volatile uint8_t kbd_time = 0;      //таймер автоповтораz
static unsigned char kbd_ts    = 0xFF;         //Текущие состояния

unsigned char kbd_state = 0;            //Внешний буфер клавиатуры
unsigned char kbd_ss    = 0xFF;         //Стабильные состояния

#define KBD_TIME_S          2
#define KBD_TIME_L          10

#define swap(A) A = (A<<4)|(A>>4)       //SWAP SWAP SWAP SWAP SWAP
//#define swap(A) #asm("SWAP R17");

void kbd_process(unsigned char st){
unsigned char temp;
temp = st & 0x0F;
kbd_ss <<= 4;                           //Сдвигаем "прошлое" на свое место
kbd_ts |= temp;                         //добавляем настоящее ... прошлое уже на своем месте
//Обработка дребезга (имеем тек. сост. в kbd_ts)
temp = kbd_ts;
swap(temp);                             //"Настоящее" на месте прошлого
kbd_ss |= (kbd_ts | (temp & 0x0F));       // tsn & tsp
kbd_ts = temp & 0xF0;                   //Наст. ставим в "прошлое"
//Определение состояний кнопок (имеем стаб. сост. в kbd_ss)
temp = kbd_ss;
swap(temp);
temp = temp ^ kbd_ss;                   //Изменившиеся состояния
temp = temp & kbd_ss;                   //7-4 нажаты 3-0 отпущены

/*Таймер работает постоянно, но так-как kbd_ss изменяется,
  то в kbd_state мы получаем нули (при не нажатых кнопках)*/
if(kbd_time == 0){                     //таймер автоповтора
  kbd_state |= ~kbd_ss & (KBD_1|KBD_0); //подсовываем будто нажаты кнопки ... причем только + и -
  kbd_time = KBD_TIME_S;                //Перезапуск таймера на кор. задержку
}else{
  kbd_time -= 1;
}

if (temp & 0xF0){                       //Если было нажатие
  kbd_state |= temp>>4;                 //поднимаем нажатые кнопки
  kbd_time = KBD_TIME_L;                //Запускаем таймер на дл. задержку
}

}
