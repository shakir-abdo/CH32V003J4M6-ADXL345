#include "adxl345.h"

// Static variables
static float scale_factor = 0.004; // Default scale factor for ±2g

// Initialize the ADXL345
bool adxl345_init(void) {
    // Initialize I2C first
    RCC->APB1PCENR |= RCC_APB1Periph_I2C1;
    RCC->APB2PCENR |= RCC_APB2Periph_GPIOC;

    // Reset I2C
    RCC->APB1PRSTR |= RCC_APB1Periph_I2C1;
    RCC->APB1PRSTR &= ~RCC_APB1Periph_I2C1;

    // Configure I2C pins (PC1=SDA, PC2=SCL)
    GPIOC->CFGLR &= ~(0xFF << (4 * 1));
    GPIOC->CFGLR |= (GPIO_CFGLR_OUT_10Mhz_AF_OD << (4 * 1)) |
                    (GPIO_CFGLR_OUT_10Mhz_AF_OD << (4 * 2));

    // Configure I2C
    I2C1->CTLR2 = 24; // 24MHz clock
    I2C1->CKCFGR = 120; // 400kHz
    I2C1->CTLR1 |= I2C_CTLR1_PE;

    // Configure ADXL345
    uint8_t config = 0x08; // Start measurement mode
    if(!i2c_write(ADXL345_ADDR, POWER_CTL_REG, config)) {
        return false;
    }

    // Set default range to ±2g
    return adxl345_setRange(ADXL345_RANGE_2G);
}

// Set the measurement range
bool adxl345_setRange(adxl345_range_t range) {
    uint8_t format = range & 0x03; // Only use the range bits

    if(!i2c_write(ADXL345_ADDR, DATA_FORMAT_REG, format)) {
        return false;
    }

    // Update scale factor based on range
    switch(range) {
        case ADXL345_RANGE_2G:  scale_factor = 0.004; break;
        case ADXL345_RANGE_4G:  scale_factor = 0.008; break;
        case ADXL345_RANGE_8G:  scale_factor = 0.016; break;
        case ADXL345_RANGE_16G: scale_factor = 0.032; break;
    }

    return true;
}

// Read acceleration data
bool adxl345_readAccel(adxl345_data_t* data) {
    int16_t x, y, z;

    if(!adxl345_readRaw(&x, &y, &z)) {
        return false;
    }

    data->x = x * scale_factor;
    data->y = y * scale_factor;
    data->z = z * scale_factor;

    return true;
}

// Read raw acceleration data
bool adxl345_readRaw(int16_t* x, int16_t* y, int16_t* z) {
    uint8_t buffer[6];

    if(!i2c_read_multi(ADXL345_ADDR, DATAX0_REG, buffer, 6)) {
        return false;
    }

    *x = (int16_t)((buffer[1] << 8) | buffer[0]);
    *y = (int16_t)((buffer[3] << 8) | buffer[2]);
    *z = (int16_t)((buffer[5] << 8) | buffer[4]);

    return true;
}

// Power down the device
void adxl345_powerDown(void) {
    i2c_write(ADXL345_ADDR, POWER_CTL_REG, 0x00);
}

// Power up the device
void adxl345_powerUp(void) {
    i2c_write(ADXL345_ADDR, POWER_CTL_REG, 0x08);
}
// Add these implementations to adxl345.c

bool i2c_write(uint8_t addr, uint8_t reg, uint8_t data) {
    uint32_t timeout = 10000;

    // Wait until I2C is not busy
    while(I2C1->STAR2 & I2C_STAR2_BUSY) {
        if(--timeout == 0) return false;
    }

    // Generate START
    I2C1->CTLR1 |= I2C_CTLR1_START;
    timeout = 10000;
    while(!(I2C1->STAR1 & I2C_STAR1_SB)) {
        if(--timeout == 0) return false;
    }

    // Send address
    I2C1->DATAR = addr << 1;
    timeout = 10000;
    while(!(I2C1->STAR1 & I2C_STAR1_ADDR)) {
        if(--timeout == 0) return false;
    }
    (void)I2C1->STAR2;

    // Send register
    I2C1->DATAR = reg;
    timeout = 10000;
    while(!(I2C1->STAR1 & I2C_STAR1_TXE)) {
        if(--timeout == 0) return false;
    }

    // Send data
    I2C1->DATAR = data;
    timeout = 10000;
    while(!(I2C1->STAR1 & I2C_STAR1_BTF)) {
        if(--timeout == 0) return false;
    }

    // Generate STOP
    I2C1->CTLR1 |= I2C_CTLR1_STOP;
    return true;
}

bool i2c_read_multi(uint8_t addr, uint8_t reg, uint8_t* data, uint8_t len) {
    uint32_t timeout = 10000;

    // Wait until I2C is not busy
    while(I2C1->STAR2 & I2C_STAR2_BUSY) {
        if(--timeout == 0) return false;
    }

    // Send register address
    if(!i2c_write(addr, reg, 0)) return false;
    Delay_Ms(1);

    // Generate START for reading
    I2C1->CTLR1 |= I2C_CTLR1_START;
    timeout = 10000;
    while(!(I2C1->STAR1 & I2C_STAR1_SB)) {
        if(--timeout == 0) return false;
    }

    // Send address for read
    I2C1->DATAR = (addr << 1) | 0x01;
    timeout = 10000;
    while(!(I2C1->STAR1 & I2C_STAR1_ADDR)) {
        if(--timeout == 0) return false;
    }
    (void)I2C1->STAR2;

    // Read data
    for(uint8_t i = 0; i < len; i++) {
        if(i == len-1) {
            I2C1->CTLR1 &= ~I2C_CTLR1_ACK;
        } else {
            I2C1->CTLR1 |= I2C_CTLR1_ACK;
        }

        timeout = 10000;
        while(!(I2C1->STAR1 & I2C_STAR1_RXNE)) {
            if(--timeout == 0) return false;
        }
        data[i] = I2C1->DATAR;
    }

    // Generate STOP
    I2C1->CTLR1 |= I2C_CTLR1_STOP;
    return true;
}
