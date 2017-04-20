#include "adc.h"

uint16_t avgSpectrum[FFT_N/2];

void ADC_Init() {
	/* AVCC as reference, mic channel is 0 */
	ADMUX = (1 << REFS0);
	/* free-running, enable interrupt, prescaler = 128 */
	ADCSRA |= (1 << ADEN) | (1 << ADATE) | (1 << ADIE) | (1 << ADPS2)
			| (1 << ADPS1) | (1 << ADPS0);
	/* start ADC */
	ADCSRA |= (1 << ADSC);
	adc.bufcnt = 0;
	adc.enabled = 1;
}

uint16_t ADC_PeakSearch(void) {
	if (!adc.newData) {
		/* ADC hasn't finished sampling */
		return 0;
	}
	/* remove DC offset of ADC signal */
	uint8_t i;
	uint32_t sum = 0;
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

	if (adc.printSpectrum) {
		/* print spectrum */
		uart_sendString_P(PSTR(VT100_ERASE_SCREEN));
		uart_sendString_P(PSTR(VT100_CURSOR_HOME));
		for (i = 0; i < FFT_N / 2; i++) {
			uint8_t j;
			for (j = 0; (j < spectrum[i]) && (j < 255); j++) {
				uart_sendByte('*');
			}
			uart_sendString_P(PSTR("\r\n"));
		}
		adc.printSpectrum = 0;
	}
	/* update average spectrum */
	for (i = 0; i < FFT_N / 2; i++) {
		avgSpectrum[i] -= avgSpectrum[i]>>2;
		avgSpectrum[i] += spectrum[i];
	}
	/* ADC can start sampling new data */
	adc.newData = 0;
	/* peak search: identify single peak (if present) */

	/* find maximum value */
	uint16_t max = 0;
	uint16_t peak = 0;
	for (i = 0; i < FFT_N / 2; i++) {
		if (avgSpectrum[i] > max) {
			max = avgSpectrum[i];
			peak = i;
		}
	}
	if(max < ADC_PEAK_MIN_HEIGHT) {
		/* peak not big enough */
		return 0;
	}
	uint32_t energy = 0;
	uint32_t energyPeak = 0;
	/* check if this is the only peak present */
	for (i = 0; i < FFT_N / 2; i++) {
		uint32_t e = avgSpectrum[i] * avgSpectrum[i];
		energy += e;
		uint8_t diff;
		if (i > peak)
			diff = i - peak;
		else
			diff = peak - i;
		if (diff <= ADC_PEAK_MAX_WIDTH_FFT) {
			energyPeak += e;
		}
	}
	if (energy * ADC_SPECTRAL_RATIO_NUM > energyPeak * ADC_SPECTRAL_RATIO_DEN) {
		/* not enough energy in peak */
		return 0;
	}

	/* a peak is present, check if main frequency is within accepted range */
	uint32_t freq = (uint32_t) peak * ADC_SAMPLE_FREQ / FFT_N;
	if (freq < 750 || freq > 2000) {
		/* peak outside of typical whistle frequency */
		return 0;
	}
	/* there is a peak in the accepted frequency range */
	/* calculate exact frequency as the weighted average of the whole peak width */
	freq = 0;
	uint16_t spectrumSum = 0;
	for (i = peak - ADC_PEAK_MAX_WIDTH_FFT; i <= peak + ADC_PEAK_MAX_WIDTH_FFT;
			i++) {
		freq += (uint32_t) avgSpectrum[i] * i * ADC_SAMPLE_FREQ / FFT_N;
		spectrumSum += avgSpectrum[i];
	}
	freq /= spectrumSum;
	return freq;
}

ISR(ADC_vect) {
	if (!adc.newData) {
		/* conversion finished, store result values */
		adc.buffer[adc.bufcnt++] = ADC;
		if (adc.bufcnt >= ADC_BUFFER_LENGTH) {
			adc.bufcnt = 0;
			adc.newData = 1;
		}
	}
}
