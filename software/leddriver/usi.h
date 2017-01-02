/**
 * \file
 * \brief Serial interface and protocol implementation
 */

#ifndef USI_H_
#define USI_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "boost.h"

/** This address must be matched in order to start listening */
#define USI_ADDRESS			0x01
/** This must be the last byte received in each transmission */
#define USI_END_IDENTIFIER	0xFF

/** Possible states of the receiver state machine */
typedef enum {
	/** Waiting for the address to be matched */
	USI_IDLE,
	/** Address matched, waiting for first byte */
	USI_ADDRESS_MATCHED,
	/** First byte received, waiting for second byte */
	USI_NEXT_BYTE,
	/** All bytes received, waiting for end identifier */
	USI_EXPECTING_END
} usi_state_t;

/** Serial receiver state and received data */
struct {
	/** Current state of the receiver state machine */
	usi_state_t state;
	/** Last received current value */
	uint16_t current;
} usi;

/**
 * \brief Initializes serial interface, activates interrupt
 */
void usi_Init(void);

/**
 * \brief Serial interrupt routine
 *
 * Is called each time a byte is received. This byte is then handled by the
 * receiver state machine.
 */
ISR(USI_OVF_vect);

#endif /* USI_H_ */
