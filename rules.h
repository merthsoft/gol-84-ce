#ifndef RULES_H
#define RULES_H

#include <stdint.h>
#include <stdlib.h>

typedef struct Rule {
	short Live;
	short Born;
	char* Name;
} Rule;

uint8_t numRules = 6;
Rule* rules;

void InitRules();

#endif