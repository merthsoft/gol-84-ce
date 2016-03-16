#ifndef RULE_H
#define RULE_H

#include <stdint.h>
#include <stdlib.h>

typedef struct Rule {
    uint8_t Live;
    uint8_t Born;
    char* Name;
} Rule;

#endif