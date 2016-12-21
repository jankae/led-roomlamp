#include "usi.h"

void usi_Init(void) {
	/* USI starts in idle state */
	usi.state = USI_IDLE;
	/* enable pull-ups on data pins */
	PORTA |= (1<<PA4) | (1<<PA6);
	/* overflow interrupt enable, three wire mode, external clock, positive edge */
	USICR |= (1 << USIOIE) | (1 << USIWM0) | (1 << USICS1);
}

ISR(USI_OVF_vect){
	/* save received data */
	uint8_t data = USIBR;
	/* clear data register (don't send 1s during next transmission,
	 * the three wire interface data out is not used) */
	USIDR = 0;
	/* clear overflow flag TODO: is this really necessary? */
	USISR = (1<<USIOIF);
	/* handle data input */
	switch (usi.state) {
	case USI_IDLE:
		/* ignore everything until address is matched */
		if (data == USI_ADDRESS)
			usi.state = USI_ADDRESS_MATCHED;
		break;
	case USI_ADDRESS_MATCHED:
		/* received first byte of 16 bit current value */
		usi.current = data;
		usi.state = USI_NEXT_BYTE;
		break;
	case USI_NEXT_BYTE:
		/* received second byte of 16 bit current value */
		usi.current += ((uint16_t) data << 8);
		usi.state = USI_EXPECTING_END;
		break;
	case USI_EXPECTING_END:
		if (data == USI_END_IDENTIFIER) {
			/* data transfer completed, set new current value */
			boost_setCurrent(usi.current);
		}
		usi.state = USI_IDLE;
	}
}
