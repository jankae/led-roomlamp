#include <avr/io.h>
#include "shell.h"

int main(void){
	uart_Init();

	sei();

	while(1){
		shell_Update();
	}
}
