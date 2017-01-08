/**
 * \file
 * \brief Boost converter regulation and safety functions
 */

#ifndef BOOST_H_
#define BOOST_H_

#include <stdlib.h>
#include <math.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "adc.h"
#include "channels.h"
#include "usi.h"

/**
 * \name Boost converter settings
 * \anchor boost_settings
 * Change these defines to adapt the boost converter to different resistor
 * values and alter limits.
 * @{
 */
/** voltage sensing factor (voltage divider factor) */
#define BOOST_VOL_SENS_FACT	40
/** current sensing shunt in mOhm */
#define BOOST_SHUNT_VALUE	1000
#define BOOST_INITIAL_VOLTAGE	37000
#define BOOST_INITIAL_CURRENT	0
#define BOOST_INITIAL_TEMPLIMIT	80
/** @} */

/**
 * \name Boost converter limits
 * Hardware measurement limitations derived from the \ref boost_settings
 * "Boost converter settings".
 *
 * DO NOT CHANGE
 * @{
 */
/** maximum voltage is defined as nearly full ADC range for the given factor */
#define BOOST_MAX_VOLTAGE	(1000UL*BOOST_VOL_SENS_FACT)
/** maximum current is defined as nearly full ADC range for the given shunt */
#define BOOST_MAX_CURRENT 	(1000000UL/BOOST_SHUNT_VALUE)
/** @} */

/**
 * Boost converter state and settings
 */
struct {
	/** Raw ADC value for boost converter current limit */
	int16_t setCurrent;
	int16_t current;
	/** Raw ADC value for boost converter voltage limit */
	uint16_t setVoltage;
	uint16_t voltage;
	/** Raw ADC value for boost converter temperature limit */
	uint8_t maxTemp;
	uint8_t temperature;
	/** selected entry in duty cycle lookup-table */
	int16_t dutyCycleIt;
	/** boost converter operating */
	uint8_t isEnabled;
	/** boost converter activated (not necessary enabled if too hot) */
	uint8_t active;
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
 * \brief Sets the temperature limit of the boost converter
 *
 * The temperature is only set with a resolution of 10°C
 *
 * \param deg Maximum temperature in °C
 */
void boost_setMaxTemperature(uint8_t deg);

/**
 * \brief Extracts the compare and top values corresponding to current duty cycle
 */
void boost_updatePWM(void);

#endif
