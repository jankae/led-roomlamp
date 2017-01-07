#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>

#define I2C_BITRATE		100000


/**
 * \brief Initializes I2C hardware
 */
void i2c_Init();

/**
 * \brief Checks whether a device responds
 *
 * \param address Address of the device
 * \return 1 if device available, 0 otherwise
 */
uint8_t i2c_CheckAddress(uint8_t address);

/**
 * \brief Writes multiple registers in an I2C device
 *
 * \param address Device address
 * \param regStart Internal register offset
 * \param registers Register content that will be written
 * \param num Number of registers to write
 * \return 0 on success, >0 on failure
 */
uint8_t i2c_WriteRegisters(uint8_t address, uint8_t regStart,
		uint8_t *registers, uint8_t num);

/**
 * \brief Reads multiple registers in an I2C device
 *
 * \param address Device address
 * \param regStart Internal register offset
 * \param registers Array the registers will be written to
 * \param num Number of registers to read
 * \return 0 on success, >0 on failure
 */
uint8_t i2c_ReadRegisters(uint8_t address, uint8_t regStart,
		uint8_t *registers, uint8_t num);

/**
 * \brief Reads one byte via I2C
 *
 * \param ack 0: Send NACK after the byte, >0: send ACK
 * \return The byte that has been read
 */
uint8_t i2c_ReadByte(uint8_t ack);

/**
 * \brief Writes one byte via I2C
 *
 * \param data The byte to be written
 * \return 0 on success, >0 on failure
 */
uint8_t i2c_WriteByte(uint8_t data);

/**
 * \brief Sends a device address
 *
 * \param address The slave address
 * \return 0 on if ACK received, 1 otherwise
 */
uint8_t i2c_SendAddress(uint8_t address);

/**
 * \brief Initiates a start condition
 *
 * \return 0 on success, >0 on failure
 */
uint8_t i2c_Start();

/**
 * \brief Initiates a stop condition
 */
void i2c_Stop();

#endif
