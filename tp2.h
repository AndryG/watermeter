#ifndef TP2_H
#define TP2_H

/* ��� ��� �������� �����
 */
typedef uint8_t tp2_ticks_t;

/* C������ min �-�� ����� �� ���������� ������� T2p
 * Tp2[] - TimePower2 - �������� ��������� "������ 2^Tp2 ���" �������� ������
 */
tp2_ticks_t tp2_calcDelay(tp2_ticks_t ticks, uint8_t Tp2);

/* ������� ������� tp2_calcDelay ��� ���� ��������
 */
tp2_ticks_t tp2_calcDelay2(tp2_ticks_t ticks, uint8_t Tp2A, uint8_t Tp2B);

/* �������� "�������� ticks �������� ��� ���������� Tp2 ���������"
 */
bool tp2_test(tp2_ticks_t ticks, uint8_t Tp2);

#endif