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

#include <graphc.h>

#include "key_helper.h"
#include "board.h"
#include "draw.h"
#include "menu.h"
#include "rule.h"

uint8_t boardWidth = 28;
uint8_t boardHeight = 28;

uint8_t cellWidth = 8;
uint8_t cellHeight = 8;

uint8_t numRules = 6;

void Settings();
void ColorSettings(MenuEventArgs* menuEventArgs);
void TopologySettings(MenuEventArgs* menuEventArgs);
void DrawSampleBoard(MenuEventArgs* menuEventArgs);
void ColorPicker(MenuEventArgs* menuEventArgs);
void InitRules();
void SaveColors(MenuEventArgs* menuEventArgs);
void DrawTopoSprite(MenuEventArgs* menuEventArgs);
void RuleSettings(MenuEventArgs* menuEventArgs);
void SetRuleMenuEvent(MenuEventArgs* menuEventArgs);

Board* mainBoard;
Board* sampleBoard;
Rule* rulesList;

#endif