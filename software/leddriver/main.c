/**
 * \file
 */
#include <avr/io.h>
#include <avr/sleep.h>

#include "boost.h"
#include "channels.h"
#include "adc.h"
#include "usi.h"

int main(void){
	boost_Init();
	boost_Enable();
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
		set_sleep_mode(SLEEP_MODE_IDLE);
		sleep_mode()
		;
	}
}
