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

	/* Setup timer1 for soft PWM */
	/* CTC mode, prescaler = 256 */
	TCCR1B |= (1<<WGM12) | (1<<WGM13) | (1<<CS12);
	/* configure as 8-bit timer */
	ICR1 = 0xff;

	/* enable interrupts */
	TIMSK1 |= (1<<OCIE1B) | (1<<OCIE1A) | (1<<TOIE1);
}

void channels_Update(uint8_t ch1, uint8_t ch2, uint8_t ch3) {
	/* clear update flag */
	channel.updateData = 0;
	/* build port turn-on masks */
	channelTmpData->turnOnMaskA = 0;
	channelTmpData->turnOnMaskB = 0;
	if (ch1) {
		channelTmpData->turnOnMaskA |= CH1_BIT;
	}
	if (ch2) {
		channelTmpData->turnOnMaskA |= CH2_BIT;
	}
	if (ch3) {
		channelTmpData->turnOnMaskB |= CH3_BIT;
	}
	/* fill compare values and turn-off masks*/
	channelTmpData->compareB = ch3;
	if (ch1 < ch2) {
		/* channel 1 turns off before channel 2 */
		channelTmpData->compareA[0] = ch1;
		channelTmpData->compareA[1] = ch2;
		channelTmpData->turnOffMaskA[0] = CH1_BIT;
		channelTmpData->turnOffMaskA[1] = CH2_BIT;
	} else {
		/* channel 2 turns off before channel 1 or at the same time */
		channelTmpData->compareA[0] = ch2;
		channelTmpData->compareA[1] = ch1;
		channelTmpData->turnOffMaskA[0] = CH2_BIT;
		channelTmpData->turnOffMaskA[1] = CH1_BIT;
		if (ch1 == ch2) {
			/* at the same time -> add CH1 to the first turn-off mask */
			channelTmpData->turnOffMaskA[0] = CH2_BIT | CH1_BIT;
		}
	}
	/* indicate new data to ISR */
	channel.updateData = 1;
}

ISR(TIM1_OVF_vect) {
	if (channel.updateData) {
		/* switch pointers -> load new PWM settings */
		channelData_t *buf = channelISRdata;
		channelISRdata = channelTmpData;
		channelTmpData = buf;
		/* load OCR0B value (is constant over whole PWM cycle) */
		OCR0B = channelISRdata->compareB;
		channel.updateData = 0;
	}
	/* load first OCR0A data */
	OCR0A = channelISRdata->compareA[0];
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
	OCR0A = channelISRdata->compareA[1];
	channel.portACnt = 1;
}
