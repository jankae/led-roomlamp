/**
 * \file
 * \brief Boost converter regulation and safety functions
 */

#ifndef BOOST_H_
#define BOOST_H_

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "adc.h"

/* voltage sensing factor (voltage divider factor) */
#define BOOST_VOL_SENS_FACT	48
/* maximum current is defined as nearly full ADC range for the given factor */
#define BOOST_MAX_VOLTAGE	(1000UL*BOOST_VOL_SENS_FACT)

/* current sensing shunt in mOhm */
#define BOOST_SHUNT_VALUE	1000
/* maximum current is defined as nearly full ADC range for the given shunt */
#define BOOST_MAX_CURRENT 	(1000000UL/BOOST_SHUNT_VALUE)

register uint8_t compare asm("r2");
register uint8_t top asm("r3");

struct {
	uint16_t setCurrentRaw;
	uint16_t setVoltageRaw;
	uint16_t PWM;
} boost;

/**
 * \brief Initializes boost converter functionality
 */
void boost_Init();

/**
 * \brief Enables the boost converter
 */
void boost_Enable();

/**
 * \brief Disables the boost converter
 */
void boost_Disable();

/**
 * \brief Updates the duty cycle/frequency to keep current/voltage constant
 *
 * Implemented as very crude two-point control
 */
void boost_Update(void);

/**
 * \brief Sets the current limit of the boost converter
 *
 * \param mA Maximum current in mA
 */
void boost_setCurrent(uint16_t mA);

/**
 * \brief Sets the voltage limit of the boost converter
 *
 * \param mV Maximum voltage in mV
 */
void boost_setMaxVoltage(uint16_t mV);

/**
 * \brief Extracts the compare and top values corresponding to current duty cycle
 */
void boost_updatePWM(void);

ISR(TIM0_OVF_vect) __attribute__ ((naked));

#endif
