#include "command.h"

const command_t commands[] PROGMEM = {
    {"help",            &command_help,          "\t\tLists all available commands"}
};

void command_parse(uint8_t argc, char *argv[]){
	uint8_t i;
	for (i = 0; i < sizeof(commands) / sizeof(command_t); i++) {
		/* iterate over all commands */
		if(command_isMatch(argv[0], commands[i].command)){
			/* found match */
			commands[i].command_function(argc, argv);
			return;
		}
	}
	/* if we reach this point no match has been found */
	uart_sendString("Unknown command.\r\n");
}

uint8_t command_isMatch(const char *commandRAM, const char *compareFLASH){
	uint8_t equal = 1;
	do {
		char c1 = *commandRAM++;
		char c2 = pgm_read_byte(compareFLASH++);
		if (c1 != c2) {
			equal = 0;
		}
		if (c1 == 0 || c2 == 0) {
			break;
		}
	} while(equal);
	return equal;
}

void command_help(uint8_t argc, char *argv[]){
	uint8_t i;
	for (i = 0; i < sizeof(commands) / sizeof(command_t); i++) {
		/* iterate over all commands */
		uart_sendString_P(commands[i].command);
		uart_sendString_P(commands[i].description);
		uart_sendString("\r\n");
	}
}
