#include "BME280.hpp"

BME280::BME280(i2c_inst_t *i2c, uint8_t address, uint8_t sda, uint8_t scl)
    : _i2c(i2c), _sensor_address(address), _sda(sda), _scl(scl) {}

BME280::~BME280() {}
// Internal Helper: Read 16-bit Little Endian
int16_t BME280::read_reg_16(uint8_t reg)
{
    uint8_t buf[2];
    i2c_write_blocking(_i2c, _sensor_address, &reg, 1, true);
    i2c_read_blocking(_i2c, _sensor_address, buf, 2, false);
    return (int16_t)((buf[1] << 8) | buf[0]);
}

int8_t BME280::read_reg_8(uint8_t reg){
    uint8_t buf[1];
    i2c_write_blocking(_i2c,_sensor_address,&reg,1,true);
    i2c_read_blocking(_i2c,_sensor_address,buf,1,false);
    return (int8_t)buf[0];
} 
int16_t BME280:: read_reg_16_special(uint8_t reg, int8_t specifier){
    uint8_t buf[2];
    i2c_write_blocking(_i2c,_sensor_address,&reg,1,true);
    i2c_read_blocking(_i2c,_sensor_address,buf,2,false);
    
    switch (specifier)
    {
    case 4:{
        uint8_t lower = buf[1] & 0x0F;
        int16_t result =(buf[0] << 4 | lower);
        return (result << 4) >> 4;
    }
    case 5:{
        int16_t result = ((buf[1] << 4) | buf[0] >> 4); 
        return (result << 4) >> 4;
        break;
    }
    default:{
        break;
    }
    }
}

void BME280::begin(uint8_t mode)
{
    _mode = mode;
    i2c_init(_i2c, 400 * 1000);
    gpio_set_function(_sda, GPIO_FUNC_I2C);
    gpio_set_function(_scl, GPIO_FUNC_I2C);
    gpio_pull_up(_sda);
    gpio_pull_up(_scl);

    // Load calibration data for Temparture
    _dig_T1 = (uint16_t)read_reg_16(DIG_T1_LOWER);
    _dig_T2 = read_reg_16(DIG_T2_LOWER);
    _dig_T3 = read_reg_16(DIG_T3_LOWER);

    // Load calibration data for Humidity
    _dig_H1 = (uint8_t)read_reg_8(DIG_H1_FULL);
    _dig_H2 = read_reg_16(DIG_H2_LOWER);
    _dig_H3 = (uint8_t)read_reg_8(DIG_H3_FULL);
    _dig_H4 = read_reg_16_special(DIG_H4_UPPER,4);
    _dig_H5 = read_reg_16_special(DIG_H5_lower,5);
    _dig_H6 = read_reg_8(DIG_H6_FULL);
}

float BME280::read_temp_c()
{
    switch (_mode)
    {
    case NORMAL:
    {
        /* code */
        break;
    }
    case FORCED:
    {
        uint8_t forced_read[] = {MEASURE_CONTROL, FORCED_READ_T1P0};
        i2c_write_blocking(_i2c, _sensor_address, forced_read, 2, true);
        break;
    }
    case SLEEP:
    {
        break;
    }
    default:
    {
        break;
    }
    }
    // 2. Wait for ready
    uint8_t status;
    uint8_t reg_status = 0xF3;
    do
    {
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

float BME280::read_temp_f()
{
    return (read_temp_c() * 1.8f) + 32.0f;
}
float BME280::read_pressure()
{
}
float BME280::read_humidity()
{
    switch (_mode)
    {
    case NORMAL:
    {
        /* code */
        break;
    }
    case FORCED:
    {
        uint8_t reg = HUMIDITY_CONTROL;
        uint8_t original_val;
        uint8_t received_data[5];
        i2c_write_blocking(_i2c, _sensor_address, &reg, 1, true);
        i2c_read_blocking(_i2c, _sensor_address, &original_val, 1, false);
        uint8_t cleared_val = (original_val & ~0x07);
        uint8_t force_val = (cleared_val | 0x01);
        uint8_t buffer[] = {HUMIDITY_CONTROL, force_val};
        i2c_write_blocking(_i2c, _sensor_address, buffer, 2, true);
        i2c_write_blocking(_i2c, _sensor_address, (uint8_t[]){MEASURE_CONTROL, FORCED_READ_T1P0}, 2, false);
        uint8_t status;
        uint8_t reg_status = 0xF3;
        do
        {
            i2c_write_blocking(_i2c, _sensor_address, &reg_status, 1, true);
            i2c_read_blocking(_i2c, _sensor_address, &status, 1, false);
        } while (status & 0x08);
        i2c_write_blocking(_i2c, _sensor_address, (uint8_t[]){TEMPERATURE}, 1, true);
        i2c_read_blocking(_i2c, _sensor_address, received_data, 5, false);

        uint32_t adc_T = ((received_data[0] << 12) | (received_data[1] << 4) | (received_data[2] >> 4));
        uint16_t adc_H = ((received_data[3] << 8) | (received_data[4]));

        int32_t var1 = ((((adc_T >> 3) - ((int32_t)_dig_T1 << 1)) * (int32_t)_dig_T2) >> 11);
        int32_t var2 = (((((adc_T >> 4) - (int32_t)_dig_T1) * ((adc_T >> 4) - (int32_t)_dig_T1)) >> 12) * (int32_t)_dig_T3) >> 14;
        _t_fine = var1 + var2;
        int32_t v_x1_u32r;

        // The Bosch Integer Formula
        v_x1_u32r = (_t_fine - ((int32_t)76800));

        v_x1_u32r = (((((adc_H << 14) - (((int32_t)_dig_H4) << 20) - (((int32_t)_dig_H5) * v_x1_u32r)) +
                       ((int32_t)16384)) >>
                      15) *
                     (((((((v_x1_u32r * ((int32_t)_dig_H6)) >> 10) *
                          (((v_x1_u32r * ((int32_t)_dig_H3)) >> 11) + ((int32_t)32768))) >>
                         10) +
                        ((int32_t)2097152)) *
                           ((int32_t)_dig_H2) +
                       8192) >>
                      14));

        v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)_dig_H1)) >> 4));

        // Step 2: Clamp the value between 0% and 100%
        // The formula can sometimes return slightly negative or >100% values due to noise
        v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
        v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);

        // Step 3: Convert to final Percentage
        // Result is in %RH as a float (e.g., 45.52)
        float final_humidity = (float)(v_x1_u32r >> 12) / 1024.0f;
        return final_humidity;

        break;
    }
    default:
    {
        break;
    }
    }
}