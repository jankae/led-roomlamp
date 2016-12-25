/**
 * \file
 * \brief Boost converter regulation and safety functions
 */

#ifndef BOOST_H_
#define BOOST_H_

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

void boost_Update(void);

void boost_setCurrent(uint16_t mA);

void boost_setMaxVoltage(uint16_t mV);

void boost_updatePWM(void);

ISR(TIM0_OVF_vect) __attribute__ ((naked));

#endif
