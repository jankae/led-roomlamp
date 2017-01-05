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
/** Maximum data packet length without address and end identifier */
#define USI_MAX_DATA		12

/** Possible states of the receiver state machine */
typedef enum {
	/** Waiting for the address to be matched */
	USI_IDLE,
	/** Address matched, waiting for first byte */
	USI_ADDRESS_MATCHED,
	/** Data packet length received, now waiting for all bytes to be transmitted */
	USI_LENGTH_RECEIVED,
	/** All bytes received, waiting for end identifier */
	USI_EXPECTING_END,
	/** Whole packet has been received */
	USI_PACKET_RECEIVED
} usi_state_t;

/** Serial receiver state and received data */
struct {
	/** Current state of the receiver state machine */
	volatile usi_state_t state;
	/** Data packet payload */
	uint8_t data[USI_MAX_DATA];
	/** Length of the current data packet */
	uint8_t length;
	/** Number of payload bytes received so far */
	uint8_t counter;
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
