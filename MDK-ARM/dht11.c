#include "dht11.h"

uint32_t pMillis, cMillis;

void microDelay(uint16_t delay)
{
  __HAL_TIM_SET_COUNTER(&htim1, 0);
  while (__HAL_TIM_GET_COUNTER(&htim1) < delay);
}

uint8_t DHT11_Start(void)
{
  uint8_t Response = 0;
  GPIO_InitTypeDef GPIO_InitStructPrivate = {0};

  GPIO_InitStructPrivate.Pin = DHT11_PIN;
  GPIO_InitStructPrivate.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructPrivate.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStructPrivate.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStructPrivate);
  HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, 0);
  HAL_Delay(20);
  HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, 1);
  microDelay(30);

  GPIO_InitStructPrivate.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructPrivate.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStructPrivate);

  microDelay(40);
  if (!(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)))
  {
    microDelay(80);
    if ((HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN))) Response = 1;
  }

  pMillis = HAL_GetTick();
  cMillis = HAL_GetTick();
  while ((HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)) && pMillis + 2 > cMillis)
  {
    cMillis = HAL_GetTick();
  }
  return Response;
}

uint8_t DHT11_Read(void)
{
  uint8_t a, b = 0;
  for (a = 0; a < 8; a++)
  {
    pMillis = HAL_GetTick();
    cMillis = HAL_GetTick();
    while (!(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)) && pMillis + 2 > cMillis)
    {
      cMillis = HAL_GetTick();
    }
    microDelay(40);
    if (!(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)))
      b &= ~(1 << (7 - a));
    else
      b |= (1 << (7 - a));

    pMillis = HAL_GetTick();
    cMillis = HAL_GetTick();
    while ((HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)) && pMillis + 2 > cMillis)
    {
      cMillis = HAL_GetTick();
    }
  }
  return b;
}