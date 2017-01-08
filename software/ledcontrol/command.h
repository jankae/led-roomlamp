#ifndef COMMAND_H_
#define COMMAND_H_

#include <stddef.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "conversions.h"
#include "uart.h"
#include "i2c.h"
#include "led.h"

typedef void (*commandHandler_t) (uint8_t, char *[]);

typedef struct {
    char command[15];
    commandHandler_t command_function;
    char description[40];
} command_t;

void command_parse(uint8_t argc, char *argv[]);

uint8_t command_isMatch(const char *commandRAM, const char *compareFLASH);

void command_help(uint8_t argc, char *argv[]);
void command_I2Cscan(uint8_t argc, char *argv[]);
void command_I2Cregister(uint8_t argc, char *argv[]);
void command_ledstats(uint8_t argc, char *argv[]);

#endif
