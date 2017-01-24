#include <avr/io.h>
#include "shell.h"
#include "timing.h"

int main(void){
	timing_Init();
	uart_Init();
	sei();

	uart_sendString_P(PSTR("led-control\n"));

	i2c_Init();
	shell_Init();

	led_Search();

	/* Turn all spots on */
	uint8_t i;
	for (i = 0; i < led.num; i++) {
		uint8_t address = led.addresses[i];
		led_SetCurrent(address, 250);
		led_UpdateSettings(address);
	}

	while(1){
		shell_Update();
	}
}
