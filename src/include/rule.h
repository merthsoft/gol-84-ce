#ifndef RULE_H
#define RULE_H

#include <stdint.h>

typedef struct {
    int Live;
    int Born;
    char* Name;
} Rule;

typedef struct {
    uint8_t NumRules;
    Rule* List;
} RulesList;

RulesList* InitRules();
void NumToRuleString(uint16_t num, char* rulesString, uint8_t* index);

#endif