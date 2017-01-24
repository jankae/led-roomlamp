#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>
#include "timing.h"

#define I2C_BITRATE		100000

#if ((F_CPU / I2C_BITRATE - 16) / 2 > 255)
#error I2C Bitrate too low
#elif ((F_CPU / I2C_BITRATE - 16) / 2 < 1)
#error I2C_Bitrate too high
#endif

typedef enum {
	I2C_OK,
	I2C_NODEVICE,
	I2C_NOACK,
	I2C_ERROR,
	I2C_TIMEOUT
} i2cResult_t;

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
i2cResult_t i2c_CheckAddress(uint8_t address);

/**
 * \brief Writes multiple registers in an I2C device
 *
 * \param address Device address
 * \param regStart Internal register offset
 * \param registers Register content that will be written
 * \param num Number of registers to write
 * \return 0 on success, >0 on failure
 */
i2cResult_t i2c_WriteRegisters(uint8_t address, uint8_t regStart,
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
i2cResult_t i2c_ReadRegisters(uint8_t address, uint8_t regStart,
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
i2cResult_t i2c_WriteByte(uint8_t data);

/**
 * \brief Sends a device address
 *
 * \param address The slave address
 * \return 0 on if ACK received, 1 otherwise
 */
i2cResult_t i2c_SendAddress(uint8_t address);

/**
 * \brief Initiates a start condition
 *
 * \return 0 on success, >0 on failure
 */
i2cResult_t i2c_Start();

/**
 * \brief Initiates a stop condition
 */
inline void i2c_Stop() {
	TWCR |= (1 << TWINT) | (1 << TWSTO);
}

#endif
