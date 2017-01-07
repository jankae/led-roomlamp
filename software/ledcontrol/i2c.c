#include "i2c.h"

void i2c_Init() {
	/* activate I2C, enable ACK */
	TWCR |= (1 << TWEN) | (1 << TWEA);
	/* set bitrate */
	TWBR = (F_CPU / I2C_BITRATE - 16) / 2;
}

uint8_t i2c_CheckAddress(uint8_t address) {
	if (i2c_Start())
		return 0;
	if (i2c_SendAddress(address))
		return 0;
	i2c_Stop();
	return 1;
}

uint8_t i2c_WriteRegisters(uint8_t address, uint8_t regStart,
		uint8_t *registers, uint8_t num) {
	if (i2c_Start())
		return 2;
	/* access device in write mode */
	if (i2c_SendAddress(address & 0xFE))
		return 1;
	/* send register address */
	if (i2c_WriteByte(regStart))
		return 1;
	/* send register values */
	for (; num > 0; num--) {
		if (i2c_WriteByte(*registers++))
			return 1;
	}
	/* all done */
	i2c_Stop();
	return 0;
}

uint8_t i2c_ReadRegisters(uint8_t address, uint8_t regStart, uint8_t *registers,
		uint8_t num) {
	if (i2c_Start())
		return 2;
	/* access device in write mode */
	if (i2c_SendAddress(address & 0xFE))
		return 1;
	/* send register address */
	if (i2c_WriteByte(regStart))
		return 1;
	if (i2c_Start())
		return 2;
	/* access device in read mode */
	if (i2c_SendAddress(address | 0x01))
		return 1;
	/* read register values */
	for (; num > 0; num--) {
		*registers++ = i2c_ReadByte(num - 1);
	}
	/* all done */
	i2c_Stop();
	return 0;
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

uint8_t i2c_WriteByte(uint8_t data) {
	TWDR = data;
	TWCR |= (1 << TWINT);
	while (!(TWCR & (1 << TWINT)))
		;
	if ((TWSR & 0xF8) != 0x28) {
		i2c_Stop();
		return 1;
	}
	return 0;
}

uint8_t i2c_SendAddress(uint8_t address) {
	TWDR = address;
	TWCR &= ~(1 << TWSTA);
	TWCR |= (1 << TWINT);
	while (!(TWCR & (1 << TWINT)))
		;
	if ((TWSR & 0xF8) != 0x18 && (TWSR & 0xF8) != 0x40) {
		i2c_Stop();
		return 1;
	}
	return 0;
}

uint8_t i2c_Start() {
	TWCR |= (1 << TWINT) | (1 << TWSTA);
	while (!(TWCR & (1 << TWINT)))
		;
	if (((TWSR & 0xF8) != 0x08) && ((TWSR & 0xF8) != 0x10))
		return 1;
	return 0;
}

void i2c_Stop() {
	TWCR |= (1 << TWINT) | (1 << TWSTO);
}
