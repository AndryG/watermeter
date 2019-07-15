// Не стал делать отдельный заголовочный файл так-как это неотъемлемая часть проекта.
// Попробую как оно с меньшим к-во файлов

// предыдущий символ
//u8 dskPrevCh;

// счетчик повторов последнего вх. символа
//u8 dskSameCnt;

static u8 prevState;

/*
  011   101   110   011
   A 001 B 100 C 010 A
   3  1  5  4  6  2  3
*/
static u8 route[7][8] = {
//      0     1     2     3     4     5     6     7 вх. символ
    {0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x03, 0x00}, // 0 z
    {0x00, 0x01, 0x01, 0x01, 0x04, 0x82, 0x01, 0x04}, // 1 A 3
    {0x00, 0x02, 0x05, 0x02, 0x02, 0x02, 0x83, 0x05}, // 2 B 5
    {0x00, 0x06, 0x03, 0x81, 0x03, 0x03, 0x03, 0x06}, // 3 C 6
    {0x00, 0x04, 0x04, 0x41, 0x04, 0x82, 0x04, 0x04}, // 4 d 4
    {0x00, 0x05, 0x05, 0x05, 0x05, 0x42, 0x83, 0x05}, // 5 e 2
    {0x00, 0x06, 0x06, 0x81, 0x06, 0x06, 0x43, 0x06}, // 6 f 1
};

#define SA_NSS    0x80 // New Stable State
#define SA_DR_RET 0x40 // Возврат с дребезга


static u8 dss; //Duration Stable State длительность стабильного состояния

//static u8 dcs; // Duration Current State

static u8 dd;  // Duration Dребезг

/* автомат обработки данных датчиков с диска
 @param три бита состояния счетчика
 @return к-во принятых импульсов 0 - диск на прежнем состоянии
*/
u8 dskAutomat(u8 ch){
  u8 s = route[prevState][ch];
  u8 state = s & 0x0f;

  switch(state){
  case 1:
  case 2:
  case 3:

    break;
  case 4:
  case 5:
  case 6:
    break;
  }

  if(s & SA_NSS){ // новое стабильное состояние
    dd = 0;
  }
  if(s & SA_DR_RET){ // возврат с дребезга
    dss += dd;
    dd = 0;
  }

    dd += 1;


  if(prevState != (s & 0x0F)){ // StateChanged

  }else{ //предыдущее состояние

  }
  prevState = state;
  return s & SA_NSS;
}/**/

