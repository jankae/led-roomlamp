#include "shell.h"

void shell_Reset(){
	shell.writePtr = 0;
	uart_sendString(" > ");
}

void shell_Update(){
	uint8_t c;
	if(!uart_getByte(&c)){
		/* no new data -> nothing to process */
		return;
	}
	/* process new data byte */
	if (shell.writePtr < SHELL_BUFFER_SIZE - 1) {
		/* there is space left in the buffer */
		if (c == '\r') {
			/* indicate end of buffer */
			shell.inputBuffer[shell.writePtr] = 0;
			/* end of input, parse received line */
			shell_InputComplete();
		} else if (c == 127 || c == 8) {
			/* remove last char from buffer */
			if (shell.writePtr > 0) {
				shell.writePtr--;
				shell.inputBuffer[shell.writePtr] = ' ';
				/* move cursor one position back */
				uart_sendString(VT100_CURSOR_LEFT);
				/* erase text after cursor */
				uart_sendString(VT100_ERASE_AFTER_CSR);
			}
		} else if (isprint(c)) {
			/* add char to buffer */
			shell.inputBuffer[shell.writePtr++] = c;
			uart_sendByte(c);
		}
	}
}

void shell_InputComplete(){
	uart_sendString("\r\n");

	if(shell.writePtr > 0){
		/* got something */
		/* extract args from line */
		uint8_t argc = 0;
		char *argv[SHELL_MAX_ARGS];
		char *ptr = (char*) shell.inputBuffer;
		do {
			/* store pointer to this argument */
			argv[argc++] = ptr;
			/* search end of argument */
			ptr = strchr(ptr, ' ');
			if(ptr) {
				/* got more arguments */
				/* mark end of this argument */
				*ptr = 0;
				/* skip leading spaces */
				while (isspace(*++ptr))
					;
			}
		} while(ptr);

		command_parse(argc, argv);
	}

	shell_Reset();
}
