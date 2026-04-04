/**
 * @file i2c.h
 * @brief STM32F1 I2C Driver
 */

#ifndef I2C_H
#define I2C_H

#include <stdint.h>

/* I2C peripheral definitions */
typedef enum {
    I2C1 = 0,
    I2C2 = 1
} I2C_Peripheral_t;

/* I2C Speed modes */
typedef enum {
    I2C_SPEED_STANDARD = 100000,   /* 100 kHz */
    I2C_SPEED_FAST = 400000        /* 400 kHz */
} I2C_Speed_t;

/**
 * @brief Initialize I2C peripheral
 * @param i2c: I2C peripheral (I2C1 or I2C2)
 * @param speed: I2C speed mode
 */
void i2cInit(I2C_Peripheral_t i2c, I2C_Speed_t speed);

/**
 * @brief Write data to I2C device
 * @param i2c: I2C peripheral
 * @param devAddr: Device address (7-bit)
 * @param regAddr: Register address
 * @param data: Data to write
 * @return: 0 on success, non-zero on error
 */
uint8_t i2cWriteReg(I2C_Peripheral_t i2c, uint8_t devAddr, uint8_t regAddr, uint8_t data);

/**
 * @brief Read data from I2C device
 * @param i2c: I2C peripheral
 * @param devAddr: Device address (7-bit)
 * @param regAddr: Register address
 * @param data: Pointer to store read data
 * @return: 0 on success, non-zero on error
 */
uint8_t i2cReadReg(I2C_Peripheral_t i2c, uint8_t devAddr, uint8_t regAddr, uint8_t *data);

/**
 * @brief Write buffer to I2C device
 * @param i2c: I2C peripheral
 * @param devAddr: Device address (7-bit)
 * @param regAddr: Register address
 * @param buffer: Data buffer
 * @param len: Number of bytes
 * @return: 0 on success, non-zero on error
 */
uint8_t i2cWriteBuffer(I2C_Peripheral_t i2c, uint8_t devAddr, uint8_t regAddr, uint8_t *buffer, uint8_t len);

/**
 * @brief Read buffer from I2C device
 * @param i2c: I2C peripheral
 * @param devAddr: Device address (7-bit)
 * @param regAddr: Register address
 * @param buffer: Buffer to store data
 * @param len: Number of bytes
 * @return: 0 on success, non-zero on error
 */
uint8_t i2cReadBuffer(I2C_Peripheral_t i2c, uint8_t devAddr, uint8_t regAddr, uint8_t *buffer, uint8_t len);

#endif /* I2C_H */