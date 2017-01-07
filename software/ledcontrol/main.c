#include <avr/io.h>
#include "shell.h"

int main(void){
	uart_Init();
	sei();

	i2c_Init();
	shell_Reset();


	while(1){
		shell_Update();
	}
}
