/**
 * \file
 * \brief Serial interface and protocol implementation
 */

#ifndef USI_H_
#define USI_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "boost.h"

#define USI_ADDRESS			0x01
#define USI_END_IDENTIFIER	0xFF

typedef enum {
	USI_IDLE, USI_ADDRESS_MATCHED, USI_NEXT_BYTE, USI_EXPECTING_END
} usi_state_t;

struct {
	usi_state_t state;
	uint16_t current;
} usi;

/**
 * \brief Initializes serial interface, activates interrupt
 */
void usi_Init(void);

ISR(USI_OVF_vect);

#endif /* USI_H_ */
