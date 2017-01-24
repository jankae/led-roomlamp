#include <avr/io.h>
#include "shell.h"
#include "timing.h"

int main(void){
	wdt_disable();
	timing_Init();
	uart_Init();
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

	while(1){
		shell_Update();
	}
}
