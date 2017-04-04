#include <avr/io.h>
#include "shell.h"
#include "timing.h"
#include "adc.h"

int main(void){
	wdt_disable();
	timing_Init();
	uart_Init();
	ADC_Init();
	sei();

	uart_sendString_P(PSTR("led-control\n"));

	i2c_Init();
	shell_Init();

	timing_Wait(10);

	led_Search();

	/* Turn all spots on */
	uint8_t i;
	for (i = 0; i < led.num; i++) {
		uint8_t address = led.addresses[i];
		led_SetCurrent(address, 250);
		timing_Wait(1);
		led_UpdateSettings(address);
		timing_Wait(1);
	}

	int16_t oldFreq = 0;
	int16_t newFreq = 0;

	while(1){
		shell_Update();
		if(adc.enabled && adc.newData) {
			oldFreq = newFreq;
			newFreq = ADC_PeakSearch();
			int16_t freq = 0;
			if(oldFreq-newFreq<50 && newFreq-oldFreq<50) {
				freq = (oldFreq + newFreq) / 2;
			}
			if(freq) {
				uint16_t current = 0;
				if(freq>2000) {
					current = LED_MAX_CURRENT;
				} else if(freq>1000) {
					freq -= 1000;
					uint32_t freqSquared = (uint32_t) freq * freq / 1000;
					current = freqSquared * LED_MAX_CURRENT / 1000;
				}
				uint8_t i;
				for (i = 0; i < led.num; i++) {
					uint8_t address = led.addresses[i];
					led_SetCurrent(address, current);
					timing_Wait(1);
					led_UpdateSettings(address);
					timing_Wait(1);
				}
			}
		}
	}
}
