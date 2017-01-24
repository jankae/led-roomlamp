/**
 * \file
 */
#include <avr/io.h>
#include <avr/sleep.h>

#include "boost.h"
#include "channels.h"
#include "adc.h"
#include "usi.h"
#include <util/delay.h>
#include "usi.h"

#define VERSION		1

int main(void){
	boost_Init();
	channels_Init();
	adc_Init();
	usi_InitI2C(I2C_ADDRESS);

	/* disable analog comparator */
	ACSR |= (1<<ACD);

	usi.ledData.version = VERSION;

	/* everything is set up, interrupts will take care of the rest */
	sei();

	while (1) {
		if(usi.ledData.control & USI_CTRL_UPDATE){
			/* new data received -> update boost converter */
			cli();
			/* copy data atomically */
			uint16_t current = usi.ledData.setCurrent;
			uint16_t voltage = usi.ledData.setVoltage;
			uint8_t temp = usi.ledData.tempLimit;
			/* clear new data flag */
			usi.ledData.control &= ~USI_CTRL_UPDATE;
			sei();
			/* set new limits for boost converter */
			boost_setCurrent(current);
			boost_setMaxVoltage(voltage);
			boost_setMaxTemperature(temp);
			if (current > 0) {
				boost.active = 1;
			} else {
				boost.active = 0;
			}
			/* update channel values */
			channels_Update(usi.ledData.channel1, usi.ledData.channel2,
					usi.ledData.channel3);
		}
		if (adc.newData) {
			boost_Update();
			adc.newData = 0;
			/* update values in USI registers */
			cli();
			if(usi.state == USI_SLAVE_IDLE){
				/* only update 16 bit values if no transaction is active */
				usi.ledData.dutyIndex = boost.dutyCycleIt;
				usi.ledData.getCurrent = boost.current;
				usi.ledData.getVoltage = boost.voltage;
			}
			sei();
			/* 8 bit values may always be updated */
			usi.ledData.compareReg = OCR0B;
			usi.ledData.topReg = OCR0A;
			usi.ledData.temperature = boost.temperature;
		}
		/* Stop condition doesn't trigger an interrupt -> poll it */
		usi_CheckForStop();

		set_sleep_mode(SLEEP_MODE_IDLE);
		sleep_mode()
		;
	}
}
