#include "aht10.h"

void AHT10_Init(I2C_HandleTypeDef* hi2c) {
	uint8_t tx_buffer[3] = {AHT10_INIT_COMMAND, 0x08, 0x00};
	HAL_Delay(20);


	HAL_StatusTypeDef res = HAL_I2C_Master_Transmit(hi2c, AHT10_I2C_ADRESS, tx_buffer, 3, 100);

	if(res != HAL_OK) {
		return;
	}

	HAL_Delay(10);
}

HAL_StatusTypeDef AHT10_Measure(I2C_HandleTypeDef *hi2c, float *humidity, float *temperature)
{
    HAL_StatusTypeDef res;
    uint8_t rx[6]; // status + 5 data bytes (total 6)


    // send trigger measurement (3 bytes)
    res = HAL_I2C_Master_Transmit(hi2c, AHT10_I2C_ADRESS, (uint8_t*)AHT10_TRIGGER_MEASUREMENT, sizeof(AHT10_TRIGGER_MEASUREMENT), 100);
    if (res != HAL_OK) return res;

    HAL_Delay(200);
     res = HAL_I2C_Master_Receive(hi2c, AHT10_I2C_ADRESS, rx, 6, 200);


    // now read the full 6 bytes (status + 5 data)
    res = HAL_I2C_Master_Receive(hi2c, AHT10_I2C_ADRESS, rx, 6, 200);
    if (res != HAL_OK) return res;

    // parse
    // rx[0] = status
    // humidity is 20 bits: rx[1] rx[2] rx[3] (bits: [rx1..rx3] >> 4)
    // temperature is 20 bits: rx[3] low nibble + rx[4] rx[5]
    uint32_t raw_h = ((uint32_t)rx[1] << 16) | ((uint32_t)rx[2] << 8) | ((uint32_t)rx[3]);
    raw_h >>= 4; // top 20 bits
    uint32_t raw_t = (((uint32_t)rx[3] & 0x0F) << 16) | ((uint32_t)rx[4] << 8) | ((uint32_t)rx[5]);

    // convert (datasheet): RH = raw_h / (2^20) * 100
    // Temperature = raw_t / (2^20) * 200 - 50
    const float denom = 1048576.0f; // 2^20
    *humidity = ((float)raw_h) * 100.0f / denom;
    *temperature = ((float)raw_t) * 200.0f / denom - 50.0f;

    return HAL_OK;
}
