#ifndef ADXL345_H
#define ADXL345_H

#include <stdbool.h>
#include <stdint.h>

#include "ch32v003fun.h"

// ADXL345 Registers
#define ADXL345_ADDR 0x53     // Device address when ALT ADDRESS pin is grounded
#define DEVID_REG 0x00        // Device ID
#define THRESH_TAP_REG 0x1D   // Tap threshold
#define OFSX_REG 0x1E         // X-axis offset
#define OFSY_REG 0x1F         // Y-axis offset
#define OFSZ_REG 0x20         // Z-axis offset
#define POWER_CTL_REG 0x2D    // Power-saving features control
#define DATA_FORMAT_REG 0x31  // Data format control
#define DATAX0_REG 0x32       // X-Axis Data 0
#define DATAX1_REG 0x33       // X-Axis Data 1
#define DATAY0_REG 0x34       // Y-Axis Data 0
#define DATAY1_REG 0x35       // Y-Axis Data 1
#define DATAZ0_REG 0x36       // Z-Axis Data 0
#define DATAZ1_REG 0x37       // Z-Axis Data 1

// Data rate codes
typedef enum {
    ADXL345_RATE_3200HZ = 0x0F,
    ADXL345_RATE_1600HZ = 0x0E,
    ADXL345_RATE_800HZ = 0x0D,
    ADXL345_RATE_400HZ = 0x0C,
    ADXL345_RATE_200HZ = 0x0B,
    ADXL345_RATE_100HZ = 0x0A,
    ADXL345_RATE_50HZ = 0x09,
    ADXL345_RATE_25HZ = 0x08
} adxl345_dataRate_t;

// Range settings
typedef enum {
    ADXL345_RANGE_2G = 0x00,
    ADXL345_RANGE_4G = 0x01,
    ADXL345_RANGE_8G = 0x02,
    ADXL345_RANGE_16G = 0x03
} adxl345_range_t;

// Acceleration data structure
typedef struct {
    float x;
    float y;
    float z;
} adxl345_data_t;

// Function prototypes
bool adxl345_init(void);
bool adxl345_setRange(adxl345_range_t range);
bool adxl345_setDataRate(adxl345_dataRate_t rate);
bool adxl345_readAccel(adxl345_data_t* data);
bool adxl345_readRaw(int16_t* x, int16_t* y, int16_t* z);
bool adxl345_selfTest(void);
void adxl345_powerDown(void);
void adxl345_powerUp(void);
bool i2c_write(uint8_t addr, uint8_t reg, uint8_t data);
bool i2c_read_multi(uint8_t addr, uint8_t reg, uint8_t* data, uint8_t len);
#endif  // ADXL345_H
