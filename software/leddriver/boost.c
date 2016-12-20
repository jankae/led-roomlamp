#include "boost.h"

void boost_Init(){
	/* setup PWM on OC0B (PA7) */
	/* initially set pin high -> FET is off */
	PORTA |= (1<<PA7);
	DDRA |= (1<<PA7);

	/* initialize default voltage/current limits */
	boost_setCurrent(0);
	boost_setMaxVoltage(41000);

	/* initialize PWM value */
	OCR0B = boost.PWM = 255;
	/* set non-inverting fast PWM mode */
	TCCR0A |= (1<<COM0B1) | (1<<WGM01) | (1<<WGM00);
	/* set PWM as fast as possible, results in 78kHz PWM frequency */
	TCCR0B |= (1<<CS00);
	/* at this point the duty cycle of the boost converter
	 * is controlled by writing to OCR0B. High values
	 * correspond to a low duty cycle (inverted FET driver),
	 * low value set a high duty cycle and thus a high output
	 * voltage. OCR0B should always be higher or equal to 70.
	 * This limits the output voltage to a maximum of about
	 * 44V.
	 */
	/* set callback to update boost converter */
	adc_setCallback(boost_Update);
}

void boost_Update(void){
	if (adc.raw[ADC_VOLTAGE] > boost.setVoltageRaw
			|| adc.raw[ADC_CURRENT] > boost.setCurrentRaw) {
		/* current or voltage at limit -> decrease duty cycle -> increase PWM */
		if (boost.PWM < BOOST_MAX_PWM)
			boost.PWM++;
	} else {
		/* not at limit -> increase duty cycle -> decrease PWM */
		if (boost.PWM > BOOST_MIN_PWM)
			boost.PWM--;
	}
	OCR0B = boost.PWM;
}

void boost_setCurrent(uint16_t mA){
	if(mA > BOOST_MAX_CURRENT)
		/* limit current to maximum */
		mA = BOOST_MAX_CURRENT;
	/* calculate raw ADC value for given current */
	uint16_t adcVoltage = ((uint32_t) mA * BOOST_SHUNT_VALUE) / 1000;
	/* convert from voltage in mV to raw ADC (10bit, 1V1 reference) */
	boost.setCurrentRaw = ((uint32_t) adcVoltage * 93) / 100;
}

void boost_setMaxVoltage(uint16_t mV){
	if(mV > BOOST_MAX_VOLTAGE)
		/* limit voltage to maximum */
		mV = BOOST_MAX_VOLTAGE;
	/* calculate raw ADC value for given voltage */
	uint16_t adcVoltage = ((uint32_t) mV * 1000) / BOOST_VOL_SENS_FACT;
	/* convert from voltage in mV to raw ADC (10bit, 1V1 reference) */
	boost.setVoltageRaw = ((uint32_t) adcVoltage * 93) / 100;
}
