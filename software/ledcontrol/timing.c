#include "timing.h"

void timing_Init(){
	/* Configure timer1 as CTC, prescaler = 1 */
	TCCR1B |= (1<<WGM12) | (1<<CS10);
	/* Set top value according to CPU frequency */
	OCR1A = (F_CPU / 1000) - 1;
	/* enable overflow interrupt */
#if __AVR_ATmega8__
	TIMSK |= (1<<OCIE1A);
#elif __AVR_ATmega1284P__
	TIMSK1 |= (1<<OCIE1A);
#elif __AVR_ATmega328P__
	TIMSK1 |= (1<<OCIE1A);
#endif
}

void timing_Wait(uint16_t ms){
	uint16_t start = timing_GetTime();
	while (start + ms > timing_GetTime())
		;
}

uint16_t timing_GetTime(){
	uint16_t buf;
	cli();
	buf = timing.ms;
	sei();
	return buf;
}

uint16_t timing_SetTimer(uint16_t timeout){
	return timing_GetTime() + timeout;
}

uint8_t timing_CheckTimer(uint16_t t) {
	if (t > timing_GetTime()) {
		return 0;
	} else {
		return 1;
	}
}

ISR(TIMER1_COMPA_vect) {
	timing.ms++;
}
