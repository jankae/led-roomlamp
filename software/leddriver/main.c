#include <avr/io.h>

#include "boost.h"
#include "channels.h"
#include "adc.h"

int main(void){
	boost_Init();
	adc_Init();
	channels_Init();
}
