/*
 * adc.h
 *
 *  Created on: Jan 6, 2017
 *      Author: jan
 */

#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "ffft.h"
#include "uart.h"
#include "VT100.h"

/** Length of ADC sampling buffer. Must be the same length as FFT_N */
#define ADC_BUFFER_LENGTH 		FFT_N
/** ADC sampling frequency in Hz */
#define ADC_SAMPLE_FREQ			9615

/** Minimum peak level */
#define ADC_PEAK_MIN_HEIGHT		64
/** Maximum width of accepted peak in Hz (one direction, e.i. half of the whole peak width */
#define ADC_PEAK_MAX_WIDTH		250UL
#define ADC_PEAK_MAX_WIDTH_FFT	(ADC_PEAK_MAX_WIDTH * FFT_N /ADC_SAMPLE_FREQ)

#define ADC_SPECTRAL_RATIO_NUM	1
#define ADC_SPECTRAL_RATIO_DEN  2


struct {
	int16_t buffer[ADC_BUFFER_LENGTH];
	uint16_t bufcnt;
	complex_t bfly_buff[FFT_N];
	volatile uint8_t newData;
	uint8_t enabled;
	uint8_t printSpectrum;
} adc;

void ADC_Init();

uint16_t ADC_PeakSearch(void);

ISR(ADC_vect);

#endif /* ADC_H_ */
