#ifndef RULE_H
#define RULE_H

#include <stdint.h>

#include "stamp.h"

typedef struct Rules {
    uint16_t Live;
    uint16_t Born;
    char* Name;
    uint8_t NumStamps;
    Stamp** Stamps;
} Rules;

typedef struct RulesList {
    uint8_t NumRules;
    Rules* List;
} RulesList;

RulesList* GetRules();
void NumToRuleString(uint16_t num, char* rulesString, uint8_t* index);

#endif