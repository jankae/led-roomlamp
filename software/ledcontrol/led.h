#ifndef LED_H_
#define LED_H_

#include <stddef.h>
#include "i2c.h"

#define LED_MAX_NUM			6
#define LED_MIN_ADDRESS		0x10
#define LED_MAX_ADDRESS		0x40
#define LED_MIN_VERSION		1
#define LED_MAX_VERSION		1

typedef struct {
	uint8_t control;
	uint8_t version;
	uint16_t setCurrent;
	uint16_t setVoltage;
	uint8_t tempLimit;
	uint8_t temperature;
	uint16_t getCurrent;
	uint16_t getVoltage;
	uint16_t dutyIndex;
	uint8_t compareReg;
	uint8_t topReg;
	uint8_t channel1;
	uint8_t channel2;
	uint8_t channel3;
} ledData_t;

#define LED_CTRL_UPDATE			0x01

struct {
	uint8_t num;
	uint8_t addresses[LED_MAX_NUM];
} led;

void led_Search(void);
i2cResult_t led_GetAllData(uint8_t address, ledData_t *data);
i2cResult_t led_SetCurrent(uint8_t address, uint16_t current);
i2cResult_t led_SetVoltage(uint8_t address, uint16_t voltage);
i2cResult_t led_SetTempLimit(uint8_t address, uint8_t limit);
i2cResult_t led_SetChannel(uint8_t address, uint8_t channel, uint8_t value);
i2cResult_t led_UpdateSettings(uint8_t address);

#endif
