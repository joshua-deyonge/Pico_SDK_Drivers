#include "BME280.hpp"


BME280::BME280(i2c_inst_t *i2c, uint8_t address, uint8_t sda, uint8_t scl)
    : _i2c(i2c), _sensor_address(address), _sda(sda), _scl(scl) {}

BME280::~BME280() {}
// Internal Helper: Read 16-bit Little Endian
int16_t BME280::read_reg_16(uint8_t reg) {
    uint8_t buf[2];
    i2c_write_blocking(_i2c, _sensor_address, &reg, 1, true);
    i2c_read_blocking(_i2c, _sensor_address, buf, 2, false);
    return (int16_t)((buf[1] << 8) | buf[0]);
}

void BME280::begin(uint8_t mode) {
    _mode = mode;
    i2c_init(_i2c, 400 * 1000);
    gpio_set_function(_sda, GPIO_FUNC_I2C);
    gpio_set_function(_scl, GPIO_FUNC_I2C);
    gpio_pull_up(_sda);
    gpio_pull_up(_scl);

    // Load calibration data
    _dig_T1 = (uint16_t)read_reg_16(DIG_T1_LOWER);
    _dig_T2 = read_reg_16(DIG_T2_LOWER);
    _dig_T3 = read_reg_16(DIG_T3_LOWER);
}

float BME280::read_temp_c() {
    switch (_mode)
    {
    case NORMAL:{
        /* code */
        break;
    }
    case FORCED:{
        uint8_t forced_read[] = {MEASURE_CONTROL ,FORCED_READ_T1P0};
        i2c_write_blocking(_i2c,_sensor_address,forced_read,2, true);
        break;
    }
    case SLEEP:{
        break;
    }
    default:{
        break;
    }
    }
    // 2. Wait for ready
    uint8_t status;
    uint8_t reg_status = 0xF3;
    do {
        i2c_write_blocking(_i2c, _sensor_address, &reg_status, 1, true);
        i2c_read_blocking(_i2c, _sensor_address, &status, 1, false);
    } while (status & 0x08);

    // 3. Read Raw Data
    uint8_t data[3];
    uint8_t reg_temp = TEMPERATURE;
    i2c_write_blocking(_i2c, _sensor_address, &reg_temp, 1, true);
    i2c_read_blocking(_i2c, _sensor_address, data, 3, false);
    int32_t adc_T = (int32_t)((data[0] << 12) | (data[1] << 4) | (data[2] >> 4));

    // 4. Compensation Math
    int32_t var1 = ((((adc_T >> 3) - ((int32_t)_dig_T1 << 1)) * (int32_t)_dig_T2) >> 11);
    int32_t var2 = (((((adc_T >> 4) - (int32_t)_dig_T1) * ((adc_T >> 4) - (int32_t)_dig_T1)) >> 12) * (int32_t)_dig_T3) >> 14;
    _t_fine = var1 + var2;
    return ((_t_fine * 5 + 128) >> 8) / 100.0f;
}

float BME280::read_temp_f() {
    return (read_temp_c() * 1.8f) + 32.0f;
}