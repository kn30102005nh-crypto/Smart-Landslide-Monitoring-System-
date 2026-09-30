#ifndef __DHT11_H__
#define __DHT11_H__

#include "main.h"

#define DHT11_PORT GPIOB
#define DHT11_PIN GPIO_PIN_9

extern TIM_HandleTypeDef htim1;

uint8_t DHT11_Start(void);
uint8_t DHT11_Read(void);
void microDelay(uint16_t delay);

#endif