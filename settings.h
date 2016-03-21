#ifndef SETTINGS_H
#define SETTINGS_H

#include "menu.h"

void Settings(Board* board, RulesList* rulesList);
void SaveSettings(Board* mainBoard, char* appVarName);
void LoadSettings(Board* mainBoard, char* appVarName);

#endif