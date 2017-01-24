#include <avr/io.h>
#include "shell.h"

int main(void){
	uart_Init();
	sei();

	uart_sendString_P(PSTR("led-control\n"));

	i2c_Init();
	shell_Init();

	while(1){
		shell_Update();
	}
}
