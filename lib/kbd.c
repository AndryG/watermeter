#include <stdint.h>
#include "kbd.h"

volatile uint8_t kbd_time = 0;      //������ �����������z
static unsigned char kbd_ts    = 0xFF;         //������� ���������

unsigned char kbd_state = 0;            //������� ����� ����������
unsigned char kbd_ss    = 0xFF;         //���������� ���������

#define KBD_TIME_S          2
#define KBD_TIME_L          10

#define swap(A) A = (A<<4)|(A>>4)       //SWAP SWAP SWAP SWAP SWAP
//#define swap(A) #asm("SWAP R17");

void kbd_process(unsigned char st){
unsigned char temp;
temp = st & 0x0F;
kbd_ss <<= 4;                           //�������� "�������" �� ���� �����
kbd_ts |= temp;                         //��������� ��������� ... ������� ��� �� ����� �����
//��������� �������� (����� ���. ����. � kbd_ts)
temp = kbd_ts;
swap(temp);                             //"���������" �� ����� ��������
kbd_ss |= (kbd_ts | temp & 0x0F);       // tsn & tsp
kbd_ts = temp & 0xF0;                   //����. ������ � "�������"
//����������� ��������� ������ (����� ����. ����. � kbd_ss)
temp = kbd_ss;
swap(temp);
temp = temp ^ kbd_ss;                   //������������ ���������
temp = temp & kbd_ss;                   //7-4 ������ 3-0 ��������

/*������ �������� ���������, �� ���-��� kbd_ss ����������,
  �� � kbd_state �� �������� ���� (��� �� ������� �������)*/
if(kbd_time == 0){                     //������ �����������
  kbd_state |= ~kbd_ss & (KBD_1|KBD_0); //����������� ����� ������ ������ ... ������ ������ + � -
  kbd_time = KBD_TIME_S;                //���������� ������� �� ���. ��������
}else{
  kbd_time -= 1;
}

if (temp & 0xF0){                       //���� ���� �������
  kbd_state |= temp>>4;                 //��������� ������� ������
  kbd_time = KBD_TIME_L;                //��������� ������ �� ��. ��������
}

}
