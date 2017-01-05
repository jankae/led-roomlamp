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

int main(void){
	boost_Init();
	channels_Init();
	adc_Init();
	usi_Init();

	/* disable analog comparator */
	ACSR |= (1<<ACD);
	/* disable timer1 module (not needed) */
	PRR |= (1<<PRTIM1);

	/* everything is set up, interrupts will take care of the rest */
	sei();

	while (1) {
		if (usi.state == USI_PACKET_RECEIVED) {
			uint16_t current;
			switch (usi.data[0]) {
			case 0:
				current = usi.data[1] + ((uint16_t) usi.data[2] << 8);
				boost_setCurrent(current);
				if (current > 0) {
					boost.active = 1;
				} else {
					boost.active = 0;
				}
				break;
			}
			usi.state = USI_IDLE;
		}
		if (adc.newData) {
			boost_Update();
			adc.newData = 0;
		}
		set_sleep_mode(SLEEP_MODE_IDLE);
		sleep_mode()
		;
	}
}
