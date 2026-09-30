#include "mpu6050.h"
#include "math.h"

int16_t ax_raw, ay_raw, az_raw;

void MPU6050_Init(void)
{
    uint8_t check;
    uint8_t data;

    HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, 0x75, 1, &check, 1, 1000);
    if (check == 0x68)
    {
        data = 0x00;
        HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, 0x6B, 1, &data, 1, 1000); // Wake up
        data = 0x07;
        HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, 0x19, 1, &data, 1, 1000); // Sample rate
        data = 0x00;
        HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, 0x1B, 1, &data, 1, 1000); // ±250 deg/s
        data = 0x00;
        HAL_I2C_Mem_Write(&hi2c2, MPU6050_ADDR, 0x1C, 1, &data, 1, 1000); // ±2g
    }
}

void MPU6050_Read_Accel(float *ax, float *ay, float *az)
{
    uint8_t data[6];
    HAL_I2C_Mem_Read(&hi2c2, MPU6050_ADDR, 0x3B, 1, data, 6, 1000);
    ax_raw = (int16_t)(data[0] << 8 | data[1]);
    ay_raw = (int16_t)(data[2] << 8 | data[3]);
    az_raw = (int16_t)(data[4] << 8 | data[5]);

    *ax = (float)ax_raw / 16384.0;
    *ay = (float)ay_raw / 16384.0;
    *az = (float)az_raw / 16384.0;
}

float fastSqrt(float number)
{
    float guess = number / 2.0f;
    for (int i = 0; i < 5; i++)
        guess = 0.5f * (guess + number / guess);
    return guess;
}
