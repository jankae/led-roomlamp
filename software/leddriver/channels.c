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
}
