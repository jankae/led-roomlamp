#include "command.h"

const command_t commands[] PROGMEM = {
		{ "help", 		&command_help, 			"\t\tLists all available commands" },
		{ "scani2c", 	&command_scanI2C,		"\t\tScans all I2C addresses" }
};

void command_parse(uint8_t argc, char *argv[]) {
	uint8_t i;
	for (i = 0; i < sizeof(commands) / sizeof(command_t); i++) {
		/* iterate over all commands */
		if (command_isMatch(argv[0], commands[i].command)) {
			/* found match */
			commandHandler_t commandHandler;
			commandHandler = (commandHandler_t) pgm_read_word(
					&commands[i].command_function);
			commandHandler(argc, argv);
			return;
		}
	}
	/* if we reach this point no match has been found */
	uart_sendString("Unknown command.\r\n");
}

uint8_t command_isMatch(const char *commandRAM, const char *compareFLASH) {
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
	} while (equal);
	return equal;
}

void command_help(uint8_t argc, char *argv[]) {
	uint8_t i;
	for (i = 0; i < sizeof(commands) / sizeof(command_t); i++) {
		/* iterate over all commands */
		uart_sendString_P(commands[i].command);
		uart_sendString_P(commands[i].description);
		uart_sendString("\r\n");
	}
}

void command_scanI2C(uint8_t argc, char *argv[]) {
	uint8_t address = 0;
	uart_sendString_P(PSTR("Scanning all I2C addresses...\r\n"));
	do {
		if(i2c_CheckAddress(address)){
			uart_sendString_P(PSTR("Address is up: 0x"));
			uart_sendByte(NIBBLE_TO_HEXCHAR(address>>4));
			uart_sendByte(NIBBLE_TO_HEXCHAR(address & 0x0F));
			if(address & 0x01){
				uart_sendString_P(PSTR(" (R)\r\n"));
			} else {
				uart_sendString_P(PSTR(" (W)\r\n"));
			}
		}
		address++;
	} while(address);
	uart_sendString_P(PSTR("...done\r\n"));
}
