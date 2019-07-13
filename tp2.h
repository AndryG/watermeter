#ifndef TP2_H
#define TP2_H

/* ��� ��� �������� �����
 */
typedef uint8_t tick_t;

/* C������ min �-�� ����� �� ���������� ������� T2p
 * Tp2[] - TimePower2 - �������� ��������� "������ 2^Tp2 ���" �������� ������
 */
tick_t tp2_calcDelay(tick_t ticks, uint8_t Tp2);

/* ������� ������� tp2_calcDelay ��� ���� ��������
 */
tick_t tp2_calcDelay2(tick_t ticks, uint8_t Tp2A, uint8_t Tp2B);

/* �������� "�������� ticks �������� ��� ���������� Tp2 ���������"
 */
bool tp2_test(tick_t ticks, uint8_t Tp2);

#endif