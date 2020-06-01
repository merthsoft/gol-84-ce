#ifndef SETTINGS_H
#define SETTINGS_H

#include "menu.h"
#include "board.h"

void Settings(Board* board, RulesList* rulesList);
void SaveSettings(Board* mainBoard, const char* appVarName);
Board* LoadSettings(const char* appVarName, RulesList* rulesList);

#endif