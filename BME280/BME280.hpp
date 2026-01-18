#pragma once

#include "pico/stdlib.h"
#include "hardware/i2c.h"

class BME280
{
private:
    /* data */
    uint8_t _sensor_address;
    i2c_inst_t* _i2c;
    uint8_t _sda;
    uint8_t _scl;
    uint16_t _dig_T1, _dig_H1, _dig_H3;
    int16_t _dig_T2, _dig_T3, _dig_H2, _dig_H4, _dig_H5;
    int8_t _dig_H6;
    int32_t _t_fine;
    uint8_t _mode;

    // Private Methods
    /**
     * @brief Used to gather Compensation Data from chip
     * @param reg regester of lower bit of Trimming Parameter
     * @returns The value of lower and upper bit registers combined
     */
    int16_t read_reg_16(uint8_t reg);

    int8_t read_reg_8(uint8_t reg);

    int16_t read_reg_16_special(uint8_t reg, int8_t specifier);

public:

    // Constructors and Deconstructors //

    /**
     * @brief Constructor for BME280 Sensor.
     * @param i2c Pointer to i2c Port used (i2c0 or i2c1)
     * @param address (Optional) Address for BME Sensor (defaults to 0x76)
     * @param sda sda pin (default 4)
     * @param scl scl pin (default 5) 
     */
    BME280(i2c_inst_t* i2c = i2c0, uint8_t address = 0x76, uint8_t sda = 4, uint8_t scl = 5);
    ~BME280();

    // Methods //

    /**
     * @brief Inializes the I2C Port, sda pin, and scl pin, and sets pullups for each.
     * @param mode NORMAL, FORCED, SLEEP (default FORCED)
     */
    void begin(uint8_t mode = FORCED);
    /**
     * @brief Requests temp read, returns value in Celcius
     */
    float read_temp_c();
    /**
     * @brief Request temp read, returns value in Farenheit
     */
    float read_temp_f();
    /**
     * @brief Unimplimented
     */
    float read_pressure();
    /**
     * @brief Unimplimented
     */
    float read_humidity();

    // Registers //

    enum BME280_Registers : uint8_t{
        //Default address for sensor
        DEFAULT_ADDRESS = 0X76,
        // Humidity- Two Registers, 16 bits
        HUMIDITY = 0xFD,
        // Tempature- Three Registers, 20 bits
        TEMPERATURE = 0xFA,
        // Pressure- Three Registers, 20 bits
        PRESSURE = 0xF7,
        // Config- settings for Standby time (t_sb), filter, and  3 wire Spi Interface (1 on, 0 off)
        CONFIG = 0xF5,
        // Measure Control (ctrl_meas)- Settings for temp and pressure sample rate, as well as mode
        MEASURE_CONTROL = 0xF4,
        // Status - Used to check if data is ready to read
        STATUS = 0xF3,
        //Humidity Control- Lower 3 bits used to set Humidity oversampling
        HUMIDITY_CONTROL = 0xF2, // DO NOT OVERWRITE OTHER BITS! 
        /*
        The following regesters are all Trimming Parameters for obtaining the compensation words to mathmatically 
        obtain the correct readings from each sensor
        */
       DIG_T1_LOWER = 0x88,
       DIG_T1_UPPER = 0x89,
       DIG_T2_LOWER = 0x8A,
       DIG_T2_UPPER = 0x8B,
       DIG_T3_LOWER = 0x8C,
       DIG_T3_UPPER = 0x8D,
       DIG_H1_FULL = 0xA1,
       DIG_H2_LOWER = 0xE1,
       DIG_H2_UPPER = 0xE2,
       DIG_H3_FULL = 0xE3,
       DIG_H4_UPPER = 0xE4,  // Upper is stored first, unlike all of the others.
       DIG_H4_LOWER = 0xE5,  // Lower 4 bits stored on lower 4 bits 
       DIG_H5_lower = 0xE5,  // Lower 4 bits stored in upper 4 bits
       DIG_H5_UPPER = 0xE6,
       DIG_H6_FULL = 0xE7


    };
    enum BME280_Control_Hex_Values: uint8_t{
        FORCED_READ_T1P0 = 0x21  // 001 (Temp x1) 000 (Press off) 01 (Forced Mode)

    };
    enum BME280_Modes: uint8_t{
        NORMAL = 0,
        FORCED = 1, 
        SLEEP = 2
    };
};
