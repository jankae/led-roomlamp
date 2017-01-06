#include "usi.h"

void usi_InitI2C(uint8_t ownAddress) {
	/* store own address */
	usi.address = ownAddress;

	usi.state = USI_SLAVE_IDLE;

	/* set SCL and SDA high */
	PORT_USI |= (1 << PORT_USI_SCL);
	PORT_USI |= (1 << PORT_USI_SDA);
	/* SCL as output */
	DDR_USI |= (1 << PORT_USI_SCL);
	/* SDA as input */
	DDR_USI &= ~(1 << PORT_USI_SDA);
	/* enable start detection interrupt, set USI to two-wire mode, clock source external */
	USICR = (1 << USISIE) | (1 << USIWM1) | (1 << USICS1);
	/* Clear all flags and reset overflow counter */
	USISR = (1 << USISIF) | (1 << USIOIF) | (1 << USIPF) | (1 << USIDC);
}

void usi_CheckForStop(void) {
	if(USISR & (1<<USIPF)){
		/* Stop flag is set */
		usi.state = USI_SLAVE_IDLE;
	}
}

ISR(USI_START_vect) {
	/* Default conditions for I2C packet */
	usi.state = USI_SLAVE_CHECK_ADDRESS;
	/* SDA as input */
	DDR_USI &= ~(1 << PORT_USI_SDA);

	/* Wait for SCL to go low to ensure the Start Condition has completed */
	while (( PIN_USI & (1 << PORT_USI_SCL)) && !(( PIN_USI & (1 << PORT_USI_SDA))))
		;
	if (!( PIN_USI & (1 << PORT_USI_SCL))) {
		/* no stop condition occured */
		/* enable overflow interrupt, switch to SCL held low after overflow */
		USICR = (1 << USISIE) | (1 << USIOIE) | (1 << USIWM1) | (1 << USIWM0)
				| (1 << USICS1);

	}
	/* Clear all flags and reset overflow counter */
	USISR = (1 << USISIF) | (1 << USIOIF) | (1 << USIPF) | (1 << USIDC);
}

ISR(USI_OVF_vect) {
	uint8_t data = 0;
	switch (usi.state) {
	case USI_SLAVE_CHECK_ADDRESS:
		/* check address and send */
		if ((USIDR == 0) || (USIDR & 0xFE) == usi.address) {
			/* general call or address matched */
			if ( USIDR & 0x01) {
				/* slave transmitter mode */
				usi.state = USI_SLAVE_SEND_DATA;
			} else {
				/* slave receiver mode */
				usi.state = USI_SLAVE_REQUEST_DATA;
				/* undefined index position */
				usi.index = 0xFF;
			}
			SET_USI_TO_SEND_ACK();
		} else {
			usi.state = USI_SLAVE_IDLE;
			SET_USI_TO_TWI_START_CONDITION_MODE();
		}
		break;
	case USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA:
		/* check for ACK and send next data if requested */
		if ( USIDR) {
			/* no ACK, master doesn't want more data */
			usi.state = USI_SLAVE_IDLE;
			SET_USI_TO_TWI_START_CONDITION_MODE();
			return;
		}
		/* if ACK -> send next byte */
		/* no break */
	case USI_SLAVE_SEND_DATA:
		/* send next data byte from buffer */
		if (usi.index == 0xFF) {
			/* this shouldn't happen -> in case of error set to 0 */
			usi.index = 0;
		}
		/* send next byte */
		USIDR = usi.data[usi.index];
		/* increment index with wrap-around */
		usi.index = (usi.index + 1) & USI_INDEX_MASK;

		usi.state = USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA;
		SET_USI_TO_SEND_DATA();
		break;

	case USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA:
		/* data has been send, prepare to read ACK */
		usi.state = USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA;
		SET_USI_TO_READ_ACK();
		break;

	case USI_SLAVE_REQUEST_DATA:
		/* ACK has been sent, now prepare to receive data */
		usi.state = USI_SLAVE_GET_DATA_AND_SEND_ACK;
		SET_USI_TO_READ_DATA();
		break;

	case USI_SLAVE_GET_DATA_AND_SEND_ACK:
		/* data has been received, store and prepare for ACK */
		data = USIDR;
		if (usi.index == 0xFF) {
			/* this is the first data after address -> set as index position */
			usi.index = data & USI_INDEX_MASK;
		} else {
			/* not the first byte -> save in buffer */
			usi.data[usi.index] = data;
			/* increment index with wrap-around */
			usi.index = (usi.index + 1) & USI_INDEX_MASK;
		}
		usi.state = USI_SLAVE_REQUEST_DATA;
		SET_USI_TO_SEND_ACK();
		break;
	}
}
