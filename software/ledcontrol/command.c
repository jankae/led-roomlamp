#include "command.h"

const command_t commands[] PROGMEM = {
		{ "help", 			&command_help,			"\t\tLists all available commands" },
		{ "reset", 			&command_reset,			"\t\tResets the mikrocontroller" },
		{ "time",			&command_time,			"\t\tPrints current system time"},
		{ "echo",			&command_echo, 			"\t\tTurn terminal echo on/off"},
		{ "i2cscan", 		&command_I2Cscan,		"\t\tScans all I2C addresses" },
//		{ "i2creg", 		&command_I2Cregister,	"\t\tReads/writes I2C registers" },
		{ "search",			&command_search,		"\t\tSearches for connected LED spots" },
		{ "ledstats",		&command_ledstats, 		"\tDisplays LED status" },
		{ "ledset",			&command_ledset, 		"\t\tSets LED values" },
		{ "light",			&command_light, 		"\t\tSets the amount of light in percent" },
		{ "whistle",		&command_whistle,		"\t\tTurn whistle control on/off"},
		{ "spectrum",		&command_spectrum,		"\tPrints the audio spectrum"},
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

void command_reset(uint8_t argc, char *argv[]){
	/* Start watchdog timer */
	wdt_enable(WDTO_30MS);
	while (1)
		;
}

void command_time(uint8_t argc, char *argv[]){
	uart_sendString_P(PSTR("System time: "));
	uint16_t time = timing_GetTime();
	uart_sendUnsignedValue(time, 10);
	uart_sendString_P(PSTR("ms\r\n"));
}

void command_echo(uint8_t argc, char *argv[]){
	if (argc == 2) {
		if (command_isMatch(argv[1], PSTR("on"))) {
			shell.echo = 1;
			return;
		} else if (command_isMatch(argv[1], PSTR("off"))) {
			shell.echo = 0;
			return;
		}
	}
	uart_sendString_P(PSTR("usage: echo on | off\r\n"));
	return;
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

void command_search(uint8_t argc, char *argv[]){
	/* Initiate new led search */
	led_Search();
	/* Print result */
	uart_sendString_P(PSTR("Found "));
	uart_sendUnsignedValue(led.num, 10);
	uart_sendString_P(PSTR(" LED spots.\r\n"));
	uint8_t i;
	for(i=0;i<led.num;i++){
		uart_sendUnsignedValue(i, 10);
		uart_sendString_P(PSTR(": 0x"));
		uart_sendUnsignedValue(led.addresses[i], 16);
		uart_sendString_P(PSTR("\r\n"));
	}
}

void command_ledstatsPrint(uint8_t address, ledData_t *data) {
	uart_sendString_P(PSTR("0x"));
	uart_sendUnsignedValue(address, 16);
	uart_sendString_P(PSTR(": LED V"));
	uart_sendUnsignedValue(data->version, 10);
	uart_sendString_P(PSTR(" Ctrl-Reg: 0x"));
	uart_sendUnsignedValue(data->control, 16);
	uart_sendString_P(PSTR(" I: ("));
	uart_sendUnsignedValue(data->getCurrent, 10);
	uart_sendByte('/');
	uart_sendUnsignedValue(data->setCurrent, 10);
	uart_sendString_P(PSTR(") V: ("));
	uart_sendUnsignedValue(data->getVoltage, 10);
	uart_sendByte('/');
	uart_sendUnsignedValue(data->setVoltage, 10);
	uart_sendString_P(PSTR(") Temp: ("));
	uart_sendUnsignedValue(data->temperature, 10);
	uart_sendByte('/');
	uart_sendUnsignedValue(data->tempLimit, 10);
	uart_sendString_P(PSTR(") Duty: "));
	uart_sendUnsignedValue(data->dutyIndex, 10);
	uart_sendByte('(');
	uart_sendUnsignedValue(data->compareReg, 10);
	uart_sendByte('/');
	uart_sendUnsignedValue(data->topReg, 10);
	uart_sendString_P(PSTR(") CH1: "));
	uart_sendUnsignedValue(data->channel1, 10);
	uart_sendString_P(PSTR(" CH2: "));
	uart_sendUnsignedValue(data->channel2, 10);
	uart_sendString_P(PSTR(" CH3: "));
	uart_sendUnsignedValue(data->channel3, 10);
	uart_sendString_P(PSTR("\r\n"));
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
		for (i = 0; i < led.num; i++) {
			uint8_t address = led.addresses[i];
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

void command_ledset(uint8_t argc, char *argv[]) {
	if (argc < 3) {
		uart_sendString_P(PSTR("usage: ledset -a | devices... flag [flag...]\r\n"
				"\tFlags:\t[-c current | -v voltage | -t temperature limit | -u\r\n"
				"\t\t-x channel1 | -y channel2 | -z channel3\r\n"));
		return;
	}
	/* parse args */
	uint8_t allLEDs = 0;
	uint16_t current = 0;
	uint16_t voltage = 0;
	uint8_t temp = 0;
	uint8_t channels[3];
#define SET_CURRENT		0x01
#define SET_VOLTAGE		0x02
#define SET_TEMPERATURE	0x04
#define SET_CHANNEL1		0x08
#define SET_CHANNEL2		0x10
#define SET_CHANNEL3		0x20
#define SET_UPDATE			0x80
	uint8_t updateFlags = 0x00;
	uint8_t i;
	for (i = 1; i < argc; i++) {
		if(argv[i][0] != '-') {
			/* not an argument -> skip */
			continue;
		}
		uint16_t param = 0;
		if (i + 1 != argc) {
			/* more args following */
			param = strtol(argv[i + 1], NULL, 0);
		}
		switch(argv[i][1]){
		case 'a':
			/* do operation on all LEDs */
			allLEDs = 1;
			break;
		case 'u':
			/* don't update changed values right away */
			updateFlags |= SET_UPDATE;
			break;
		case 'c':
			/* update current */
			current = param;
			updateFlags |= SET_CURRENT;
			i++;
			break;
		case 'v':
			/* update voltage */
			voltage = param;
			updateFlags |= SET_VOLTAGE;
			i++;
			break;
		case 't':
			/* update temperature limit */
			temp = param;
			updateFlags |= SET_TEMPERATURE;
			i++;
			break;
		case 'x':
			/* update channel1 limit */
			channels[0] = param;
			updateFlags |= SET_CHANNEL1;
			i++;
			break;
		case 'y':
			/* update channel2 limit */
			channels[1] = param;
			updateFlags |= SET_CHANNEL2;
			i++;
			break;
		case 'z':
			/* update channel3 limit */
			channels[2] = param;
			updateFlags |= SET_CHANNEL3;
			i++;
			break;
		default:
			uart_sendString_P(PSTR("Unknown option: -"));
			uart_sendByte(argv[i][1]);
			uart_sendString_P(PSTR("\r\n"));
			return;
		}
	}

	if (current == 0xffff && voltage == 0xffff && temp == 0xff
			&& !(updateFlags & SET_UPDATE)) {
		/* no operation specified */
		uart_sendString_P(PSTR("No operation specified.\r\n"));
		return;
	}

	/* iterate over all addresses */
	for (i = 0; i < led.num; i++) {
		uint8_t address = led.addresses[i];
		if (!allLEDs) {
			/* -a hasn't been specified, set address */
			uint8_t j;
			for (j = 1; j < argc; j++) {
				/* abort search on first option */
				if (argv[j][0] == '-') {
					/* mark end of search */
					j = argc;
					break;
				}
				if (((uint8_t) strtol(argv[j], NULL, 0) & 0xFE) == address) {
					/* address matched, execute operations */
					break;
				}
			}
			if (j == argc) {
				/* no match found */
				continue;
			}
		}
		/* either -a is specified or address matched */
		/* execute specified operations */
		i2cResult_t res = I2C_OK;
		if (updateFlags & SET_CURRENT) {
			res = led_SetCurrent(address, current);
		}
		if (updateFlags & SET_VOLTAGE) {
			res = led_SetVoltage(address, voltage);
		}
		if (updateFlags & SET_TEMPERATURE) {
			res = led_SetTempLimit(address, temp);
		}
		if (updateFlags & SET_TEMPERATURE) {
			res = led_SetTempLimit(address, temp);
		}
		if (updateFlags & SET_CHANNEL1) {
			res = led_SetChannel(address, 1, channels[0]);
		}
		if (updateFlags & SET_CHANNEL2) {
			res = led_SetChannel(address, 2, channels[1]);
		}
		if (updateFlags & SET_CHANNEL3) {
			res = led_SetChannel(address, 3, channels[2]);
		}
		if (updateFlags & SET_UPDATE) {
			res = led_UpdateSettings(address);
		}
		if (res == I2C_OK) {
			uart_sendString_P(PSTR("0x"));
			uart_sendValue(address, 16);
			uart_sendString_P(PSTR(": OK\r\n"));
		} else if(!allLEDs){
			/* only print failure messages if address has been explicitly specified */
			uart_sendString_P(PSTR("0x"));
			uart_sendValue(address, 16);
			uart_sendString_P(PSTR(": FAILED\r\n"));
		}
	}
}

void command_light(uint8_t argc, char *argv[]){
	if (argc != 2) {
		uart_sendString_P(PSTR("usage: light percent\r\n"));
		return;
	}
	/* extract percentage from parameter */
	uint32_t percent = strtol(argv[1], NULL, 0);
	if(percent>100) {
		uart_sendString_P(PSTR("Unable to set more than 100%\r\n"));
		return;
	}
	/* scale percentage to current */
	uint16_t current = LED_MAX_CURRENT * percent / 100;
	/* iterate over all addresses */
	uint8_t i;
	for (i = 0; i < led.num; i++) {
		uint8_t address = led.addresses[i];
		led_SetCurrent(address, current);
		led_UpdateSettings(address);
	}
}

void command_whistle(uint8_t argc, char *argv[]){
	if (argc == 2) {
		if (command_isMatch(argv[1], PSTR("on"))) {
			adc.enabled = 1;
			return;
		} else if (command_isMatch(argv[1], PSTR("off"))) {
			adc.enabled = 0;
			return;
		}
	}
	uart_sendString_P(PSTR("usage: whistle on | off\r\n"));
	return;
}

void command_spectrum(uint8_t argc, char *argv[]) {
	adc.printSpectrum = 1;
}
