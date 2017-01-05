/**
 * \file
 * \brief Serial interface and protocol implementation
 */
#include "usi.h"

void usi_Init(void) {
	/* USI starts in idle state */
	usi.state = USI_IDLE;
	/* enable pull-ups on data pins */
	PORTA |= (1 << PA4) | (1 << PA6);
	/* overflow interrupt enable, three wire mode, external clock, positive edge */
	USICR |= (1 << USIOIE) | (1 << USIWM0) | (1 << USICS1);
	/* reset clock counter */
	USISR = 0;
}

ISR(USI_OVF_vect) {
	/* save received data */
	uint8_t data = USIBR;
	/* clear data register (don't send 1s during next transmission,
	 * the three wire interface data out is not used) */
	USIDR = 0;
	/* handle data input */
	switch (usi.state) {
	case USI_IDLE:
		/* ignore everything until address is matched */
		if (data == USI_ADDRESS) {
			usi.state = USI_ADDRESS_MATCHED;
		}
		break;
	case USI_ADDRESS_MATCHED:
		/* receive data length */
		if (data <= USI_MAX_DATA) {
			usi.length = data;
			usi.counter = 0;
			usi.state = USI_LENGTH_RECEIVED;
		} else {
			/* data packet is too long -> reject */
			usi.state = USI_IDLE;
		}
		break;
	case USI_LENGTH_RECEIVED:
		/* receive payload byte */
		usi.data[usi.counter] = data;
		usi.counter++;
		if (usi.counter >= usi.length) {
			/* whole payload received */
			usi.state = USI_EXPECTING_END;
		}
		break;
	case USI_EXPECTING_END:
		if (data == USI_END_IDENTIFIER) {
			/* data transfer completed */
			usi.state = USI_PACKET_RECEIVED;
		} else {
			usi.state = USI_IDLE;
		}
		break;
	case USI_PACKET_RECEIVED:
		/* received packet not yet handled -> ignore further data */
		break;
	}
}
