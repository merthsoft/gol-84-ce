#ifndef MAIN_H
#define MAIN_H

/* Keep these headers */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

/* Standard headers - it's recommended to leave them included */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <debug.h>

#include "key_helper.h"
#include "board.h"
#include "draw.h"
#include "menu.h"
#include "rule.h"

void NumToRuleString(uint16_t num, char* rulesString, uint8_t* index);
void DrawPlayPauseIcon(bool running);

#endif