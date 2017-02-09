#include "adc.h"

void ADC_Init(){
	/* AVCC as reference, mic channel is 0 */
	ADMUX = (1<<REFS0);
	/* free-running, enable interrupt, prescaler = 128 */
	ADCSRA |= (1<<ADEN) | (1<<ADFR) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1);
	/* start ADC */
	ADCSRA |= (1 << ADSC);
	adc.bufcnt = 0;
}

uint16_t ADC_PeakSearch(void){
	if (!adc.newData) {
		/* ADC hasn't finished sampling */
		return 0;
	}
	/* remove DC offset of ADC signal */
	uint8_t i;
	uint16_t sum = 0;
	for (i = 0; i < ADC_BUFFER_LENGTH; i++) {
		sum += adc.buffer[i];
	}
	uint16_t avg = sum / ADC_BUFFER_LENGTH;
	for (i = 0; i < ADC_BUFFER_LENGTH; i++) {
		adc.buffer[i] -= avg;
	}
	/* perform FFT analysis of ADC signal */
	fft_input(adc.buffer, adc.bfly_buff);
	fft_execute(adc.bfly_buff);
	/* use ADC buffer as result of ADC -> saves some RAM */
	uint16_t *spectrum = (uint16_t*) adc.buffer;
	fft_output(adc.bfly_buff, spectrum);
	/* peak search: identify single peak (if present) */

	/* find maximum value */
	uint16_t max = 0;
	uint16_t peak = 0;
	for (i = 0; i < FFT_N / 2; i++) {
		if (spectrum[i] > max) {
			max = spectrum[i];
			peak = i;
		}
	}
	/* check if this is the only peak present */
	for (i = 0; i < FFT_N / 2; i++) {
		if (spectrum[i] > max / ADC_PEAK_OFF_PEAK_RATIO) {
			/* found another frequency with high power */
			/* check if it is still the same peak */
			uint8_t diff;
			if (i > peak)
				diff = i - peak;
			else
				diff = peak - i;
			if (diff > ADC_PEAK_MAX_WIDTH_FFT) {
				/* different peak -> no identifiable main peak */
				adc.newData = 0;
				return 0;
			}
		}
	}
	/* a peak is present, check if main frequency is within accepted range */
	uint32_t freq = (uint32_t) peak * ADC_SAMPLE_FREQ / FFT_N;
	if(freq < 750 || freq > 2000) {
		/* peak outside of typical whistle frequency */
		adc.newData = 0;
		return 0;
	}
	/* there is a peak in the accepted frequency range */
	/* calculate exact frequency as the weighted average of the whole peak width */
	freq = 0;
	uint16_t spectrumSum = 0;
	for (i = peak - ADC_PEAK_MAX_WIDTH_FFT; i <= peak + ADC_PEAK_MAX_WIDTH_FFT;
			i++) {
		freq += (uint32_t) spectrum[i] * i * ADC_SAMPLE_FREQ / FFT_N;
		spectrumSum += spectrum[i];
	}
	adc.newData = 0;
	freq /= spectrumSum;
	return freq;
}

ISR(ADC_vect) {
	if (!adc.newData) {
		/* conversion finished, store result values */
		adc.buffer[adc.bufcnt++] = ADC;
		if (adc.bufcnt == ADC_BUFFER_LENGTH) {
			adc.bufcnt = 0;
			adc.newData = 1;
		}
	}
}
