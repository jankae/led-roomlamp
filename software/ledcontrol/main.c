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

	while(1){
		shell_Update();
	}
}
