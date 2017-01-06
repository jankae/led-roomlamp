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
	usi_InitI2C(0b00110100);

	/* disable analog comparator */
	ACSR |= (1<<ACD);
	/* disable timer1 module (not needed) */
	PRR |= (1<<PRTIM1);

	usi.data[USI_REG_R_VERSION] = VERSION;

	/* everything is set up, interrupts will take care of the rest */
	sei();

	while (1) {
		if(usi.data[USI_REG_W_CTRL] & USI_CTRL_UPDATE){
			/* new data received -> update boost converter */
			cli();
			/* copy data atomically */
			uint16_t current = usi.data[USI_REG_W_CURRENT_LOW]
					+ ((uint16_t) usi.data[USI_REG_W_CURRENT_LOW] << 8);
			uint16_t voltage = usi.data[USI_REG_W_VOLTAGE_LOW]
					+ ((uint16_t) usi.data[USI_REG_W_VOLTAGE_LOW] << 8);
			uint8_t temp = usi.data[USI_REG_W_TEMP_LIMIT];
			/* clear new data flag */
			usi.data[USI_REG_W_CTRL] &= ~USI_CTRL_UPDATE;
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
		}
		if (adc.newData) {
			boost_Update();
			adc.newData = 0;
		}
		/* Stop condition doesn't trigger an interrupt -> poll it */
		usi_CheckForStop();
		/* choose deepest sleep mode possible with current state */
		if (!boost.active && usi.state == USI_SLAVE_IDLE) {
			set_sleep_mode(SLEEP_MODE_PWR_SAVE);
		} else {
			set_sleep_mode(SLEEP_MODE_IDLE);
		}
		sleep_mode()
		;
	}
}
