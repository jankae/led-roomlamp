/**
 * \file
 * \brief ADC setup and free-running operation
 */
#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include <avr/interrupt.h>

/**
 * \name ADC hardware channels
 * Actual hardware channel numbers
 * @{
 */
#define ADC_CURRENT_CH		0
#define ADC_VOLTAGE_CH		3
#define	ADC_TEMP_CH			5
/** @} */

/**
 * \name ADC software channels
 * Software channel numbers. Must start at 0 and ADC_NUM_CHANNELS must always be last.
 * @{
 */
#define ADC_VOLTAGE			0
#define ADC_CURRENT			1
#define ADC_TEMPERATURE		2
#define ADC_NUM_CHANNELS	3
/** @} */

/** ADC data and state of ADC conversion state machine */
struct {
	/** contains automatically updated raw ADC values for all channels */
	uint16_t raw[ADC_NUM_CHANNELS];

	/** Callback function which will be called after each conversion cycle */
	void (*callback)(void);

	/** Contains the currently sampled software channel number */
	uint8_t cnt;
	/** Stores the lastly sampled software channel number */
	uint8_t lastCnt;

	/** Maps the hardware channel number to the software channels */
	uint8_t channels[ADC_NUM_CHANNELS];
} adc;

/**
 * \brief Initializes ADC
 *
 * Sets up the channels, the reference and starts the ADC in free-running mode
 */
void adc_Init();

/**
 * \brief Sets the callback function
 *
 * \param cb Callback function to be called (NULL pointer if no function should be called)
 */
void adc_setCallback(void (*cb)());

/**
 * \brief ADC interrupt routine
 *
 * Stores the result from the last conversion and starts conversion of next channel.
 * If all channels have been sampled, the callback function (if set) will be called.
 */
ISR(ADC_vect);

#endif
