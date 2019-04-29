#ifndef _KBD_INCLUDED_
#define _KBD_INCLUDED_

extern unsigned char kbd_ss;    //������� ����� ���������� "���������� ���������"
extern unsigned char kbd_state; //������� ����� ���������� "������ ������"
#define KBD_0           0x01
#define KBD_1           0x02
#define KBD_2           0x04
#define KBD_3           0x08

void kbd_process(unsigned char st);
#endif
