// �� ���� ������ ��������� ������������ ���� ���-��� ��� ������������ ����� �������.
// �������� ��� ��� � ������� �-�� ������

// ���������� ������
uint8_t dskPrevCh;

// ������� �������� ���������� ��. �������
uint8_t dskSameCnt;

static uint8_t state;

/*
  011   101   110   011
   A 001 B 100 C 010 A
   3  1  5  4  6  2  3
*/
static uint8_t route[7][8] = {
//      0     1     2     3     4     5     6     7 ��. ������
    {0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x03, 0x00}, // 0 z
    {0x00, 0x01, 0x01, 0x01, 0x04, 0x82, 0x01, 0x04}, // 1 A 3
    {0x00, 0x02, 0x05, 0x02, 0x02, 0x02, 0x83, 0x05}, // 2 B 5
    {0x00, 0x06, 0x03, 0x81, 0x03, 0x03, 0x03, 0x06}, // 3 C 6
    {0x00, 0x04, 0x04, 0x41, 0x04, 0x82, 0x04, 0x04}, // 4 d 4
    {0x00, 0x05, 0x05, 0x05, 0x05, 0x42, 0x83, 0x05}, // 5 e 2
    {0x00, 0x06, 0x06, 0x81, 0x06, 0x06, 0x43, 0x06}, // 6 f 1
};

/*static uint8_t revers[3][3] => {
  {}, // 0
  {}, // 1
  {}, // 2
};*/

/*
static uint8_t route[8][8] = {
//   0  1  2  3  4  5  6  ��. ������
    {0, 1, 2, 3, 4, 5, 6, 7},  // 0 - 000 �����
    {0, 1, 1, 1, 1, 5, 1, 7},  // 1 - 001 a-b
    {0, 2, 2, 3, 2, 2, 2, 7},  // 2 - 010 c-a
    {0, 1, 3, 3, 3, 5, 3, 7},  // 3 - 011 A
    {0, 4, 4, 4, 4, 4, 6, 7},  // 4 - 100 b-c
    {0, 5, 5, 5, 4, 5, 6, 7},  // 5 - 101 B
    {0, 6, 2, 3, 6, 6, 6, 7},  // 6 - 110 C
    {0, 1, 2, 3, 4, 5, 6, 7},  // 7 - 111 ��������
};

static bool res[8] = {0, 0, 0, 1, 0, 1, 1, 0};

/* ������� ��������� ������ �������� � �����
 @param ��� ���� ��������� ��������
 @return �-�� �������� ��������� 0 - ���� �� ������� ���������
*/
uint8_t dskAutomat(uint8_t ch){
  uint8_t prev = state;
  state = route[state][ch];
  return prev != state;
}
/*
uint8_t dskAutomat2(uint8_t ch){
  uint8_t prev = state;
  state = route[state][ch];
  // ������� ������� ��. ����
  if(dskPrevCh == ch){
    dskSameCnt += 1;
  }else{
    dskPrevCh  = ch;
    dskSameCnt = 0;
  }
  // ��� ����� ������ ��� ���?
  if(prev != state){
    return res[state];
  }else{
    return false;
  }
}/**/

