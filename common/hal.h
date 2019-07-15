#ifndef HAL_H
#define HAL_H

#define SNR_PORT_LED  PORTD
#define SNR_PORT_IN   PORTC
#define SNR_PORT_OUT  PORTC

#define SNR_CH0_IN    PC0 // PC0-PC1
#define SNR_CH0_OUT   PC4
#define SNR_CH0_LED   PD4 // PD4-PD5

#define SNR_CH1_IN    PC2 // PC2-PC3
#define SNR_CH1_OUT   PC5
#define SNR_CH1_LED   PD6 // PD6-PD7

#define LED_PORT      PORTD
#define LED_PIN       PD2


// поломался автомат sensorTask
#define E_SENSOR_TASK_1   1
#define E_QT_OVERFLOW     9

#define QT_QUEUE_OVERFLOW_CODE  E_QT_OVERFLOW

#endif