/**
 * \file
 * \brief Defines and routines for accessing the auxiliary channels
 */
#include "channels.h"

channelData_t channelData1, channelData2;
channelData_t *channelISRdata, *channelTmpData;

void channels_Init(void){
	/* set external channel control pins as outputs */
	CH1_DDR |= (1<<CH1_BIT);
	CH2_DDR |= (1<<CH2_BIT);
	CH3_DDR |= (1<<CH3_BIT);

	channelISRdata = &channelData1;
	channelTmpData = &channelData2;

//	OCR1B = 150;
//	channelISRdata->turnOnMaskB |= (1<<CH3_BIT);

	/* Setup timer1 for soft PWM */
	/* CTC mode, prescaler = 256 */
	TCCR1B |= (1<<WGM12) | (1<<WGM13) | (1<<CS12);
	/* configure as 8-bit timer */
	ICR1 = 0xff;

	/* enable interrupts */
	TIMSK1 |= (1<<OCIE1B) | (1<<OCIE1A) | (1<<ICIE1);
}

void channels_Update(uint8_t ch1, uint8_t ch2, uint8_t ch3) {
	/* clear update flag */
	channel.updateData = 0;
	/* build port turn-on masks */
	channelTmpData->turnOnMaskA = 0;
	channelTmpData->turnOnMaskB = 0;
	if (ch1) {
		channelTmpData->turnOnMaskA |= (1<<CH1_BIT);
	}
	if (ch2) {
		channelTmpData->turnOnMaskA |= (1<<CH2_BIT);
	}
	if (ch3) {
		channelTmpData->turnOnMaskB |= (1<<CH3_BIT);
	}
	/* fill compare values and turn-off masks*/
	channelTmpData->compareB = ch3;
	/* get min and max compare A values and bitmasks */
	uint8_t min, max, bitMaskMin, bitMaskMax;
	if (ch1 < ch2) {
		min = ch1;
		max = ch2;
		bitMaskMin = (1 << CH1_BIT);
		bitMaskMax = (1 << CH2_BIT);
	} else {
		min = ch2;
		max = ch1;
		bitMaskMin = (1 << CH2_BIT);
		bitMaskMax = (1 << CH1_BIT);
	}
	/* fill channel data with min/max values */
	channelTmpData->compareA[0] = min;
	channelTmpData->compareA[1] = max;
	channelTmpData->turnOffMaskA[0] = bitMaskMin;
	channelTmpData->turnOffMaskA[1] = bitMaskMax;
	if (min == 0 || min == max) {
		/* either both channels have the same value
		 * 		-> single compare match A interrupt
		 * or the 'min'-channel is turned off
		 * 		-> just one interrupt
		 */
		channelTmpData->compareA[0] = max;
		channelTmpData->turnOffMaskA[0] |= bitMaskMax;
	}
	/* indicate new data to ISR */
	channel.updateData = 1;
}

ISR(TIM1_CAPT_vect) {
	if (channel.updateData) {
		/* switch pointers -> load new PWM settings */
		channelData_t *buf = channelISRdata;
		channelISRdata = channelTmpData;
		channelTmpData = buf;
		/* load OCR1B value (is constant over whole PWM cycle) */
		OCR1B = channelISRdata->compareB;
		channel.updateData = 0;
	}
	/* load first OCR0A data */
	OCR1A = channelISRdata->compareA[0];
	channel.portACnt = 0;
	PORTA |= channelISRdata->turnOnMaskA;
	PORTB |= channelISRdata->turnOnMaskB;
}

ISR(TIM1_COMPB_vect) {
	/* turn off channel 3 */
	CH3_Off();
}

ISR(TIM1_COMPA_vect) {
	PORTA &= ~channelISRdata->turnOffMaskA[channel.portACnt];
	OCR1A = channelISRdata->compareA[1];
	channel.portACnt = 1;
}
