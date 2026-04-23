/**
 * @file i2c.h
 * @brief I2C Driver - Inter-Integrated Circuit
 *
 * Chức năng: Giao tiếp I2C với các cảm biến/thiết bị
 * Hỗ trợ: I2C1 (PB6=SCL, PB7=SDA), I2C2 (PB10=SCL, PB11=SDA)
 * Tốc độ: 100kHz (Standard) hoặc 400kHz (Fast)
 */

#ifndef I2C_H
#define I2C_H

#include <stdint.h>

/* ========== Định nghĩa peripheral ========== */
typedef enum {
    I2C1 = 0,
    I2C2 = 1
} I2C_Peripheral_t;

/* ========== Tốc độ I2C ========== */
typedef enum {
    I2C_SPEED_STANDARD = 100000,   /* 100 kHz */
    I2C_SPEED_FAST = 400000        /* 400 kHz */
} I2C_Speed_t;

/* ========== Hàm API ========== */

/**
 * @brief Khởi tạo I2C peripheral
 * @param i2c: I2C1 hoặc I2C2
 * @param speed: Tốc độ (I2C_SPEED_STANDARD hoặc I2C_SPEED_FAST)
 */
void i2cInit(I2C_Peripheral_t i2c, I2C_Speed_t speed);

/**
 * @brief Ghi 1 byte vào thanh ghi thiết bị
 * @param i2c: I2C peripheral
 * @param devAddr: Địa chỉ thiết bị (7-bit)
 * @param regAddr: Địa chỉ thanh ghi trên thiết bị
 * @param data: Giá trị ghi
 * @return: 0 = thành công, 1 = lỗi timeout
 */
uint8_t i2cWriteReg(I2C_Peripheral_t i2c, uint8_t devAddr, uint8_t regAddr, uint8_t data);

/**
 * @brief Đọc 1 byte từ thanh ghi thiết bị
 * @param i2c: I2C peripheral
 * @param devAddr: Địa chỉ thiết bị (7-bit)
 * @param regAddr: Địa chỉ thanh ghi
 * @param data: Con trỏ lưu dữ liệu đọc được
 * @return: 0 = thành công, 1 = lỗi timeout
 */
uint8_t i2cReadReg(I2C_Peripheral_t i2c, uint8_t devAddr, uint8_t regAddr, uint8_t *data);

/**
 * @brief Ghi nhiều bytes vào thanh ghi thiết bị
 * @param i2c: I2C peripheral
 * @param devAddr: Địa chỉ thiết bị (7-bit)
 * @param regAddr: Địa chỉ thanh ghi
 * @param buffer: Buffer chứa dữ liệu ghi
 * @param len: Số bytes cần ghi
 * @return: 0 = thành công, 1 = lỗi timeout
 */
uint8_t i2cWriteBuffer(I2C_Peripheral_t i2c, uint8_t devAddr, uint8_t regAddr, uint8_t *buffer, uint8_t len);

/**
 * @brief Đọc nhiều bytes từ thanh ghi thiết bị
 * @param i2c: I2C peripheral
 * @param devAddr: Địa chỉ thiết bị (7-bit)
 * @param regAddr: Địa chỉ thanh ghi
 * @param buffer: Buffer lưu dữ liệu đọc được
 * @param len: Số bytes cần đọc
 * @return: 0 = thành công, 1 = lỗi timeout
 */
uint8_t i2cReadBuffer(I2C_Peripheral_t i2c, uint8_t devAddr, uint8_t regAddr, uint8_t *buffer, uint8_t len);

#endif /* I2C_H */
