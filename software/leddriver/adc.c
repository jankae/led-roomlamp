#include "adc.h"

void adc_Init(){
	/* initialize channel array */
	adc.channels[ADC_VOLTAGE] = ADC_VOLTAGE_CH;
	adc.channels[ADC_CURRENT] = ADC_CURRENT_CH;
	adc.channels[ADC_TEMPERATURE] = ADC_TEMP_CH;

	/* initialize ADC sampling at channel 1 */
	adc.cnt = 1;
	adc.lastCnt = 0;

	/* set 1.1V internal reference and select first channel */
	ADMUX |= (1 << REFS1) | adc.channels[0];

	/* enable the ADC, free-running, interrupt enable, prescaler = 128 */
	ADCSRA |= (1 << ADEN) | (1 << ADATE) | (1 << ADIE) | (1 << ADPS2)
			| (1 << ADPS1) | (1 << ADPS0);

	/* start the first conversion */
	ADCSRA |= (1<<ADSC);
}

void adc_setCallback(void (*cb)()){
	adc.callback = cb;
}

ISR(ADC_vect){
	/* a conversion finished */
	/* store conversion result */
	adc.raw[adc.lastCnt] = ADC;
	/* switch to next channel */
	adc.cnt = (adc.cnt + 1) % ADC_NUM_CHANNELS;
	adc.lastCnt = (adc.lastCnt + 1) % ADC_NUM_CHANNELS;
	/* set next channel for next conversion */
	ADMUX |= (1 << REFS1) | adc.channels[adc.cnt];
	if (adc.callback && !adc.lastCnt){
		/* ADC cycle finished */
		adc.callback();
	}
}
