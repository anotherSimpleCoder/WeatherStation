#include "stm32f1xx_hal.h"

#define AHT10_I2C_ADRESS 			(0x38 << 1)

#define AHT10_INIT_COMMAND			0b11100001
#define AHT10_TRIGGER_MEASUREMENT 	0b10101100
#define AHT10_SOFT_RESET			0b10111010


void AHT10_Init();
HAL_StatusTypeDef AHT10_Measure(I2C_HandleTypeDef *hi2c, float *humidity, float *temperature);
