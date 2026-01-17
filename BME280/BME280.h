#pragma once

#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Default settings
#define BME280_ADDR 0x76

// Function Prototypes
void bme280_init(i2c_inst_t *i2c, uint sda, uint scl);
float bme280_read_temp_c(i2c_inst_t *i2c);
float bme280_read_temp_f(i2c_inst_t *i2c);