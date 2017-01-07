#ifndef SHELL_H_
#define SHELL_H_

#include <string.h>
#include <ctype.h>
#include "uart.h"
#include "command.h"
#include "VT100.h"

#define SHELL_BUFFER_SIZE				128

#define SHELL_MAX_ARGS					16

struct {
	uint8_t inputBuffer[SHELL_BUFFER_SIZE];
	uint8_t writePtr;
} shell;

void shell_Reset();

void shell_Update();

void shell_InputComplete();


#endif
