/**
 * \file
 * \brief Defines and routines for accessing the auxiliary channels
 */
#ifndef CHANNELS_H_
#define CHANNELS_H_

#include <avr/io.h>

/**
 * \name Port defines for the three auxiliary channels
 * @{
 */
#define CH1_DDR			DDRA
#define CH1_PORT		PORTA
#define CH1_BIT			PA2

#define CH2_DDR			DDRA
#define CH2_PORT		PORTA
#define CH2_BIT			PA1

#define CH3_DDR			DDRB
#define CH3_PORT		PORTB
#define CH3_BIT			PB2
/** @} */

/**
 * \name Turn on/off macros for auxiliary channels
 * @{
 */
#define CH1_On()		CH1_PORT |= (1<<CH1_BIT)
#define CH1_Off()		CH1_PORT &= ~(1<<CH1_BIT)

#define CH2_On()		CH1_PORT |= (1<<CH1_BIT)
#define CH2_Off()		CH1_PORT &= ~(1<<CH1_BIT)

#define CH3_On()		CH1_PORT |= (1<<CH1_BIT)
#define CH4_Off()		CH1_PORT &= ~(1<<CH1_BIT)
/** @} */

/**
 * \brief Initializes the control pins for the external channels
 */
void channels_Init(void);

#endif
