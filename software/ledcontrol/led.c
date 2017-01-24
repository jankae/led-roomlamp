#include "led.h"

void led_Search(void) {
	led.num = 0;
	uint8_t address;
	/* Scan I2C addresses for LED spots */
	for (address = LED_MIN_ADDRESS; address <= LED_MAX_ADDRESS; address += 2) {
		ledData_t data;
		if (led_GetAllData(address, &data) != I2C_OK) {
			/* no device at this address */
			continue;
		}
		if (data.version >= LED_MIN_VERSION && data.version <= LED_MAX_VERSION) {
			/* found an LED spot */
			led.addresses[led.num] = address;
			led.num++;
			if (led.num >= LED_MAX_NUM) {
				/* Can't store more spots */
				break;
			}
		}
	}
}

i2cResult_t led_GetAllData(uint8_t address, ledData_t *data) {
	return i2c_ReadRegisters(address, 0, (uint8_t*) data, sizeof(ledData_t));
}
i2cResult_t led_SetCurrent(uint8_t address, uint16_t current) {
	return i2c_WriteRegisters(address, offsetof(ledData_t, setCurrent),
			(uint8_t*) &current, sizeof(current));
}
i2cResult_t led_SetVoltage(uint8_t address, uint16_t voltage) {
	return i2c_WriteRegisters(address, offsetof(ledData_t, setVoltage),
			(uint8_t*) &voltage, sizeof(voltage));
}
i2cResult_t led_SetTempLimit(uint8_t address, uint8_t limit) {
	return i2c_WriteRegisters(address, offsetof(ledData_t, tempLimit),
			(uint8_t*) &limit, sizeof(limit));
}
i2cResult_t led_SetChannel(uint8_t address, uint8_t channel, uint8_t value){
	if(channel<1 || channel>3){
		/* channel is out of bounds */
		return I2C_NOACK;
	}
	return i2c_WriteRegisters(address,
			offsetof(ledData_t, channel1) - 1 + channel, &value, sizeof(value));
}
i2cResult_t led_UpdateSettings(uint8_t address) {
	/* read control register */
	uint8_t control;
	uint8_t res = i2c_ReadRegisters(address, offsetof(ledData_t, control),
			&control, 1);
	if (res) {
		/* failure during read */
		return res;
	}
	/* set data update bit */
	control |= LED_CTRL_UPDATE;
	/* write control register */
	return i2c_WriteRegisters(address, offsetof(ledData_t, control), &control,
			1);
}
