/**
 * \file
 * \brief Defines and routines for accessing the auxiliary channels
 */
#include "channels.h"

void channels_Init(void){
	/* set external channel control pins as outputs */
	CH1_DDR |= (1<<CH1_BIT);
	CH2_DDR |= (1<<CH2_BIT);
	CH3_DDR |= (1<<CH3_BIT);

#if CH1_SOFTUART
	CH1_On();
#endif
}

#if CH1_SOFTUART
void CH1_SendString(uint8_t *s){
	while(*s){
		CH1_SendChar(*s++);
	}
}

void CH1_SendChar(uint8_t c){
	uint16_t data = ((uint16_t) c) << 1 | 0x600;
	uint8_t i;
	for (i = 0; i < 11; i++) {
		if (data & 0x01) {
			CH1_On();
		} else {
			CH1_Off();
		}
		data >>= 1;
		_delay_us(103);
	}
}
#endif
