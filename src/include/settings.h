#ifndef SETTINGS_H
#define SETTINGS_H

#include "menu.h"
#include "board.h"

void Settings(Board* board, RulesList* rulesList);
void SaveSettings(Board* board, const char* appVarName);
void LoadSettings(Board* board, const char* appVarName, RulesList* rulesList);

#endif