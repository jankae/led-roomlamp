#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#define ADC_CURRENT_CH		0
#define ADC_VOLTAGE_CH		3
#define	ADC_TEMP_CH			5

#define ADC_VOLTAGE			0
#define ADC_CURRENT			1
#define ADC_TEMPERATURE		2
#define ADC_NUM_CHANNELS	3

struct {
	/* contains automatically updated raw ADC values for all three channels */
	uint16_t raw[ADC_NUM_CHANNELS];

	void (*callback)(void);

	uint8_t cnt;
	uint8_t lastCnt;
	uint8_t channels[ADC_NUM_CHANNELS];
} adc;

void adc_Init();

void adc_setCallback(void (*cb)());

ISR(ADC_vect);

#endif
