#ifndef __MPU6050_H__
#define __MPU6050_H__

#include "main.h"

#define MPU6050_ADDR 0xD0

extern I2C_HandleTypeDef hi2c2;

void MPU6050_Init(void);
void MPU6050_Read_Accel(float *ax, float *ay, float *az);
float fastSqrt(float number);

#endif