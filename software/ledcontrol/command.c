#include "command.h"

const command_t commands[] PROGMEM = {
		{ "help", 			&command_help,			"\t\tLists all available commands" },
		{ "i2cscan", 		&command_I2Cscan,		"\t\tScans all I2C addresses" },
		{ "i2creg", 		&command_I2Cregister,	"\t\tReads/writes I2C registers" },
		{ "ledstats",		&command_ledstats, 		"\tDisplays LED status" }
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
	uart_sendString("Unknown command. Try 'help'\r\n");
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

void command_I2Cscan(uint8_t argc, char *argv[]) {
	uint8_t address = 0;
	uart_sendString_P(PSTR("Scanning all I2C addresses...\r\n"));
	do {
		if (i2c_CheckAddress(address) == I2C_OK) {
			uart_sendString_P(PSTR("Address is up: 0x"));
			uart_sendByte(NIBBLE_TO_HEXCHAR(address >> 4));
			uart_sendByte(NIBBLE_TO_HEXCHAR(address & 0x0F));
			if (address & 0x01) {
				uart_sendString_P(PSTR(" (R)\r\n"));
			} else {
				uart_sendString_P(PSTR(" (W)\r\n"));
			}
		}
		address++;
	} while (address);
	uart_sendString_P(PSTR("...done\r\n"));
}

void command_I2Cregister(uint8_t argc, char *argv[]) {
	uint8_t device = 0;
	uint8_t offset = 0;
	uint8_t write = 0;
	uint8_t format = 0;
	/* parse arguments */
	uint8_t i;
	for (i = 0; i < argc; i++) {
		if (command_isMatch(argv[i], PSTR("-w"))) {
			/* it is a write operation */
			write = 1;
		} else if (command_isMatch(argv[i], PSTR("-d"))) {
			if (i + 1 == argc) {
				/* no next arg -> missing device */
				uart_sendString_P(PSTR("ERROR: -d without device address\r\n"));
			} else {
				/* try to parse argument */
				i++;
				device = strtol(argv[i], NULL, 0);
			}
		} else if (command_isMatch(argv[i], PSTR("-o"))) {
			if (i + 1 == argc) {
				/* no next arg -> missing device */
				uart_sendString_P(PSTR("ERROR: -o without offset\r\n"));
			} else {
				/* try to parse argument */
				i++;
				offset = strtol(argv[i], NULL, 0);
			}
		} else if (command_isMatch(argv[i], PSTR("-f"))) {
			if (i + 1 == argc) {
				/* no next arg -> missing device */
				uart_sendString_P(PSTR("ERROR: -f without format string\r\n"));
			} else {
				/* next argument is the format string*/
				format = ++i;
			}
		}
	}
	if (!format) {
		uart_sendString_P(PSTR("No format string given.\r\n"));
		return;
	}
	/* parse format string to determine size */
	uint8_t size = 0;
#define COMMAND_I2CREG_MAX_LENGTH	32
	uint8_t paramArray[COMMAND_I2CREG_MAX_LENGTH];
	uint8_t fcnt = 0;
	while (argv[format][fcnt]) {
		/* check for and read corresponding value */
		int16_t arg = 0;
		if (write) {
			/* check whether an argument is given for this parameter */
			if (argc < format + fcnt + 2) {
				uart_sendString_P(
						PSTR("Not enough arguments for format string.\r\n"));
				return;
			}
			arg = strtol(argv[format + fcnt + 1], NULL, 0);
		}
		/* accumulate size */
		switch (argv[format][fcnt]) {
		case 'd':
		case 'i':
		case 'u':
			if (size < COMMAND_I2CREG_MAX_LENGTH - 2) {
				*(int16_t*) &paramArray[size] = (int16_t) arg;
			}
			size += 2;
			break;
		case 'x':
		case 'c':
			if (size < COMMAND_I2CREG_MAX_LENGTH - 1) {
				*(uint8_t*) &paramArray[size - 1] = (uint8_t) arg;
			}
			size ++;
			break;
		case 's':
			if (size < COMMAND_I2CREG_MAX_LENGTH - 1) {
				*(int8_t*) &paramArray[size - 1] = (int8_t) arg;
			}
			size ++;
			break;
		default:
			uart_sendString_P(PSTR("Unsupported format: "));
			uart_sendByte(argv[format][fcnt]);
			uart_sendString_P(PSTR("\r\n"));
			return;
		}
		fcnt++;
	}
	/* read/write operation */
	i2cResult_t res;
	if (write) {
		res = i2c_WriteRegisters(device, offset, paramArray, size);
	} else {
		res = i2c_ReadRegisters(device, offset, paramArray, size);
	}
	if (res != I2C_OK) {
		uart_sendString_P(PSTR("I2C operation failed.\r\n"));
		return;
	}

	if (!write) {
		/* print read register content */
		fcnt = 0;
		uint8_t *val = paramArray;
		while (argv[format][fcnt]) {
			/* accumulate size */
			switch (argv[format][fcnt]) {
			case 'd':
			case 'i':
				uart_sendValue(*(int16_t*) val, 10);
				val += 2;
				break;
			case 'u':
				uart_sendValue(*(uint16_t*) val, 10);
				val += 2;
				break;
			case 'x':
				uart_sendString_P(PSTR("0x"));
				uart_sendValue(*(uint8_t*) val, 16);
				val++;
				break;
			case 'c':
				uart_sendValue(*(int8_t*) val, 10);
				val++;
				break;
			case 's':
				uart_sendValue(*(uint8_t*) val, 10);
				val++;
				break;
			}
			uart_sendString_P(PSTR("\r\n"));
			fcnt++;
		}
	}
}

void command_ledstatsPrint(uint8_t address, ledData_t *data) {
	uart_sendString_P(PSTR("0x"));
	uart_sendValue(address, 16);
	uart_sendString_P(PSTR(": LED V"));
	uart_sendValue(data->version, 10);
	uart_sendString_P(PSTR(" Ctrl-Reg: 0x"));
	uart_sendValue(data->control, 16);
	uart_sendString_P(PSTR(" I: ("));
	uart_sendValue(data->getCurrent, 10);
	uart_sendByte('/');
	uart_sendValue(data->setCurrent, 10);
	uart_sendString_P(PSTR(") V: ("));
	uart_sendValue(data->getVoltage, 10);
	uart_sendByte('/');
	uart_sendValue(data->setVoltage, 10);
	uart_sendString_P(PSTR(") Temp: ("));
	uart_sendValue(data->temperature, 10);
	uart_sendByte('/');
	uart_sendValue(data->tempLimit, 10);
	uart_sendString_P(PSTR(") Duty: "));
	uart_sendValue(data->dutyIndex, 10);
	uart_sendByte('(');
	uart_sendValue(data->compareReg, 10);
	uart_sendByte('/');
	uart_sendValue(data->topReg, 10);
	uart_sendString_P(PSTR(")\r\n"));
}

void command_ledstats(uint8_t argc, char *argv[]) {
	if (argc < 2) {
		uart_sendString_P(PSTR("Usage: ledstats -a | devices...\r\n"));
		return;
	}
	ledData_t data;
	/* check whether -a is specified */
	if (argc == 2 && command_isMatch(argv[1], PSTR("-a"))) {
		/* try to read all addresses */
		uint8_t i;
		for (i = 1; i <= 127; i++) {
			uint8_t address = (i * 2) + 1;
			if (i2c_ReadRegisters(address, 0, (uint8_t*) &data, sizeof(data))
					== I2C_OK) {
				command_ledstatsPrint(address, &data);
			}
		}
	} else {
		/* iterate over all args and use them as I2C addresses */
		uint8_t i;
		for (i = 1; i < argc; i++) {
			/* parse address */
			uint8_t address = strtol(argv[i], NULL, 0);
			if (!address) {
				/* couldn't parse address */
				uart_sendString_P(PSTR("Invalid device address: "));
				uart_sendString(argv[i]);
				uart_sendString_P(PSTR("\r\n"));
				continue;
			}
			/* try to read from address */
			if (i2c_ReadRegisters(address, 0, (uint8_t*) &data, sizeof(data))
					== I2C_OK) {
				command_ledstatsPrint(address, &data);
			} else {
				/* no I2C response */
				uart_sendString(argv[i]);
				uart_sendString_P(PSTR(": I2C read failed.\r\n"));
			}
		}
	}
}
