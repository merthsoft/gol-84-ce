#ifndef RULES_H
#define RULES_H

#include <stdint.h>
#include <stdlib.h>

typedef struct {
	short live;
	short born;
	char* name;
} rule;

uint8_t numRules = 6;
rule* rules;
short live = 0x0C;
short born = 0x08;

void InitRules();

#endif