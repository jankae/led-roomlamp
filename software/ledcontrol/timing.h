#ifndef TIMING_H_
#define TIMING_H_

#include <stdint.h>
#include <avr/interrupt.h>

struct {
	uint16_t ms;
} timing;

void timing_Init();

void timing_Wait(uint16_t ms);

uint16_t timing_GetTime();

uint16_t timing_SetTimer(uint16_t timeout);

uint8_t timing_CheckTimer(uint16_t t);

#endif
