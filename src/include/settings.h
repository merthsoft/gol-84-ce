#ifndef SETTINGS_H
#define SETTINGS_H

#include "menu.h"
#include "board.h"

void Settings(Board* board, RulesList* rulesList);
void SaveSettings(Board* mainBoard, char* appVarName);
Board* LoadSettings(char* appVarName);

#endif