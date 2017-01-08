#include "i2c.h"

void i2c_Init() {
	/* activate I2C, enable ACK */
	TWCR |= (1 << TWEN) | (1 << TWEA);
	/* set bitrate */
	TWBR = (F_CPU / I2C_BITRATE - 16) / 2;
}

i2cResult_t i2c_CheckAddress(uint8_t address) {
	i2cResult_t res;
	res = i2c_Start();
	if (res == I2C_OK) {
		res = i2c_SendAddress(address);
		if(res == I2C_OK && (address & 0x01)){
			/* it was a read address -> do a dummy read */
			i2c_ReadByte(0);
		}
	}
	i2c_Stop();
	return res;
}

i2cResult_t i2c_WriteRegisters(uint8_t address, uint8_t regStart,
		uint8_t *registers, uint8_t num) {
	i2cResult_t res;
	res = i2c_Start();
	/* access device in write mode */
	if (res == I2C_OK) {
		res = i2c_SendAddress(address & 0xFE);
	}
	/* send register address */
	if (res == I2C_OK) {
		res = i2c_WriteByte(regStart);
	}
	/* send register values */
	for (; num > 0; num--) {
		if (res == I2C_OK) {
			res = i2c_WriteByte(*registers++);
		}
	}
	/* all done */
	i2c_Stop();
	return res;
}

i2cResult_t i2c_ReadRegisters(uint8_t address, uint8_t regStart,
		uint8_t *registers, uint8_t num) {
	i2cResult_t res;
	res = i2c_Start();
	/* access device in write mode */
	if (res == I2C_OK) {
		res = i2c_SendAddress(address & 0xFE);
	}
	/* send register address */
	if (res == I2C_OK) {
		res = i2c_WriteByte(regStart);
	}
	/* send repeated start */
	if (res == I2C_OK) {
		res = i2c_Start();
	}
	/* access device in read mode */
	if (res == I2C_OK) {
		res = i2c_SendAddress(address | 0x01);
	}
	/* read register values */
	if (res == I2C_OK) {
		for (; num > 0; num--) {
			*registers++ = i2c_ReadByte(num - 1);
		}
	}
	/* all done */
	i2c_Stop();
	return res;
}

uint8_t i2c_ReadByte(uint8_t ack) {
	if (ack)
		TWCR |= (1 << TWEA);
	else
		TWCR &= ~(1 << TWEA);
	TWCR |= (1 << TWINT);
	while (!(TWCR & (1 << TWINT)))
		;
	return TWDR;
}

i2cResult_t i2c_WriteByte(uint8_t data) {
	TWDR = data;
	TWCR |= (1 << TWINT);
	while (!(TWCR & (1 << TWINT)))
		;
	if ((TWSR & 0xF8) != 0x28) {
		return I2C_NOACK;
	}
	return I2C_OK;
}

i2cResult_t i2c_SendAddress(uint8_t address) {
	TWDR = address;
	TWCR &= ~(1 << TWSTA);
	TWCR |= (1 << TWINT);
	while (!(TWCR & (1 << TWINT)))
		;
	if ((TWSR & 0xF8) != 0x18 && (TWSR & 0xF8) != 0x40) {
		return I2C_NODEVICE;
	}
	return I2C_OK;
}

i2cResult_t i2c_Start() {
	TWCR |= (1 << TWINT) | (1 << TWSTA);
	while (!(TWCR & (1 << TWINT)))
		;
	if (((TWSR & 0xF8) != 0x08) && ((TWSR & 0xF8) != 0x10))
		return I2C_ERROR;
	return I2C_OK;
}
