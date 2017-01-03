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

/**
 * \name Boost converter settings
 * \anchor boost_settings
 * Change these defines to adapt the boost converter to different resistor
 * values and alter limits.
 * @{
 */
/** voltage sensing factor (voltage divider factor) */
#define BOOST_VOL_SENS_FACT	48
/** current sensing shunt in mOhm */
#define BOOST_SHUNT_VALUE	1000
/** maximum allowed temperature in °C */
#define BOOST_MAX_TEMP		80
/** NTC Reference resistance (at 25°C) in Ohm */
#define BOOST_NTC_REF_R		10000
/** constant resistor value in NTC voltage divider in Ohm */
#define BOOST_NTC_LOW_R		3300
/** NTC constant A (see datasheet) */
#define BOOST_NTC_CONST_A	-14.6337f
/** NTC constant B (see datasheet) */
#define BOOST_NTC_CONST_B	4791.842f
/** NTC constant C (see datasheet) */
#define BOOST_NTC_CONST_C	-115334.0f
/** NTC constant D (see datasheet) */
#define BOOST_NTC_CONST_D	-3730535.0f
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

/** maximum temperature in kelvin */
#define BOOST_NTC_MAX_K		(BOOST_MAX_TEMP+273.16f)
/** NTC resistance at specific temperature in kelvin */
#define BOOST_NTC_R(temp)	((long)(BOOST_NTC_REF_R*exp(BOOST_NTC_CONST_A \
							+BOOST_NTC_CONST_B/(temp) \
							+BOOST_NTC_CONST_C/((temp)*(temp)) \
							+BOOST_NTC_CONST_D/((temp)*(temp)*(temp)))))
/** ADC voltage at NTC divider at specific temperature in °C */
#define BOOST_NTC_ADC_V(temp)	(5000UL*BOOST_NTC_LOW_R/(BOOST_NTC_LOW_R \
								+BOOST_NTC_R((temp##UL)+273.16f)))
/** @} */

/**
 * \name Fixed registers
 * The compare and top values of the PWM generating counter
 * are assigned permanently to registers. This way, they can be
 * transferred to the corresponding counter registers very fast
 * and enable the usage of a "naked ISR".
 *
 * @{
 */
register uint8_t compare asm("r2");
register uint8_t top asm("r3");
/** @} */

/**
 * Boost converter state and settings
 */
struct {
	/** Raw ADC value for boost converter current limit */
	uint16_t setCurrentRaw;
	/** Raw ADC value for boost converter voltage limit */
	uint16_t setVoltageRaw;
	/** Raw ADC value for boost converter temperature limit */
	uint16_t maxTempRaw;
	/** selected entry in duty cycle lookup-table */
	uint16_t dutyCycleIt;
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

/**
 * \brief Timer Overflow routine
 *
 * Called when the PWM generating timer reaches the "top" value and new
 * compare/top values have been calculated. This function copies the compare/top
 * values into the corresponding timer registers.
 */
ISR(TIM0_OVF_vect) __attribute__ ((naked));

#endif
