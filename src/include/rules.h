#ifndef RULE_H
#define RULE_H

#include <stdint.h>

#include "stamp.h"

typedef struct {
    uint16_t Live;
    uint16_t Born;
    char* Name;
    uint8_t NumStamps;
    Stamp* Stamps;
} Rules;

typedef struct {
    uint8_t NumRules;
    Rules* List;
} RulesList;

RulesList* InitRules();
void NumToRuleString(uint16_t num, char* rulesString, uint8_t* index);

#endif