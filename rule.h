#ifndef RULE_H
#define RULE_H

#include <stdint.h>
#include <stdlib.h>

typedef struct Rule {
    int Live;
    int Born;
    char* Name;
} Rule;

uint8_t InitRules(Rule** rulesList);


#endif