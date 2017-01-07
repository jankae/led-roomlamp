#ifndef COMMAND_H_
#define COMMAND_H_

#include <avr/pgmspace.h>
#include "uart.h"

typedef struct {
    char command[15];
    void (*command_function) (uint8_t, char *[]);
    char description[40];
} command_t;

void command_parse(uint8_t argc, char *argv[]);

uint8_t command_isMatch(const char *commandRAM, const char *compareFLASH);

void command_help(uint8_t argc, char *argv[]);


#endif
