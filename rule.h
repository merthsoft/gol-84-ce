#ifndef RULE_H
#define RULE_H

#include <stdint.h>
#include <stdlib.h>

typedef struct Rule {
    int Live;
    int Born;
    char* Name;
} Rule;

typedef struct RulesList {
    uint8_t NumRules;
    Rule* List;
} RulesList;

RulesList* InitRules();
void NumToRuleString(uint16_t num, char* rulesString, uint8_t* index);

#endif