/**
 * \file
 * \brief ADC setup and free-running operation
 */
#include "adc.h"

void adc_Init(){
	/* initialize channel array */
	adc.channels[ADC_VOLTAGE] = ADC_VOLTAGE_CH;
	adc.channels[ADC_CURRENT] = ADC_CURRENT_CH;
	adc.channels[ADC_TEMPERATURE] = ADC_TEMP_CH;

	adc.newData = 0;
	adc.cycleCnt = 0;

	/* initialize ADC sampling at channel 1 */
	adc.cnt = 1;
	adc.lastCnt = 0;

	/* select first channel */
	ADMUX = adc.channels[0] | (1 << REFS1);

	/* enable the ADC, free-running, interrupt enable, prescaler = 128 */
	ADCSRA |= (1 << ADEN) | (1 << ADATE) | (1 << ADIE) | (1 << ADPS2)
			| (1 << ADPS1) | (1 << ADPS0);

	/* start the first conversion */
	ADCSRA |= (1<<ADSC);
}

ISR(ADC_vect){
	/* a conversion finished */
	/* store conversion result */
	adc.rawSum[adc.lastCnt] += ADC;
	/* switch to next channel */
	adc.cnt = (adc.cnt + 1) % ADC_NUM_CHANNELS;
	adc.lastCnt = (adc.lastCnt + 1) % ADC_NUM_CHANNELS;
	/* set next channel for next conversion */
	ADMUX = adc.channels[adc.cnt] | (1 << REFS1);
	if (!adc.lastCnt) {
		/* last channel has been measured */
		adc.cycleCnt++;
		if (adc.cycleCnt >= 8) {
			adc.raw[0] = adc.rawSum[0] >> 3;
			adc.raw[1] = adc.rawSum[1] >> 3;
			adc.raw[2] = adc.rawSum[2] >> 3;
			adc.newData = 1;
			adc.rawSum[0] = 0;
			adc.rawSum[1] = 0;
			adc.rawSum[2] = 0;
			adc.cycleCnt = 0;
		}
	}
}
