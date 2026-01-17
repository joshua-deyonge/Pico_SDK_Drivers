#include "BME280.h"

// Internal calibration variables
static uint16_t dig_T1;
static int16_t dig_T2, dig_T3;
static int32_t t_fine;

// Internal Helper: Read 16-bit Little Endian
static int16_t read_reg_16(i2c_inst_t *i2c, uint8_t reg) {
    uint8_t buf[2];
    i2c_write_blocking(i2c, BME280_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c, BME280_ADDR, buf, 2, false);
    return (int16_t)((buf[1] << 8) | buf[0]);
}

void bme280_init(i2c_inst_t *i2c, uint sda, uint scl) {
    i2c_init(i2c, 400 * 1000);
    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_pull_up(scl);

    // Load calibration data
    dig_T1 = (uint16_t)read_reg_16(i2c, 0x88);
    dig_T2 = read_reg_16(i2c, 0x8A);
    dig_T3 = read_reg_16(i2c, 0x8C);
}

float bme280_read_temp_c(i2c_inst_t *i2c) {
    // 1. Trigger Forced Measurement
    uint8_t config[] = {0xF4, 0x21};
    i2c_write_blocking(i2c, BME280_ADDR, config, 2, false);

    // 2. Wait for ready
    uint8_t status;
    uint8_t reg_status = 0xF3;
    do {
        i2c_write_blocking(i2c, BME280_ADDR, &reg_status, 1, true);
        i2c_read_blocking(i2c, BME280_ADDR, &status, 1, false);
    } while (status & 0x09);

    // 3. Read Raw Data
    uint8_t data[3];
    uint8_t reg_temp = 0xFA;
    i2c_write_blocking(i2c, BME280_ADDR, &reg_temp, 1, true);
    i2c_read_blocking(i2c, BME280_ADDR, data, 3, false);
    int32_t adc_T = (int32_t)((data[0] << 12) | (data[1] << 4) | (data[2] >> 4));

    // 4. Compensation Math
    int32_t var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1)) * (int32_t)dig_T2) >> 11);
    int32_t var2 = (((((adc_T >> 4) - (int32_t)dig_T1) * ((adc_T >> 4) - (int32_t)dig_T1)) >> 12) * (int32_t)dig_T3) >> 14;
    t_fine = var1 + var2;
    return ((t_fine * 5 + 128) >> 8) / 100.0f;
}

float bme280_read_temp_f(i2c_inst_t *i2c) {
    return (bme280_read_temp_c(i2c) * 1.8f) + 32.0f;
}