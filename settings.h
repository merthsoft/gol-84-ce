#ifndef SETTINGS_H
#define SETTINGS_H

#include "menu.h"

void Settings(Board* board, RulesList* rulesList);
void ColorSettings(MenuEventArgs* menuEventArgs);
void TopologySettings(MenuEventArgs* menuEventArgs);
void DrawSampleBoard(MenuEventArgs* menuEventArgs);
void ColorPicker(MenuEventArgs* menuEventArgs);
void SaveColors(MenuEventArgs* menuEventArgs);
void DrawTopoSprite(MenuEventArgs* menuEventArgs);
void RuleSettings(MenuEventArgs* menuEventArgs);
void SetRuleMenuEvent(MenuEventArgs* menuEventArgs);
void CellSizeSettings(MenuEventArgs* menuEventArgs);
void RandomPercentageSettings(MenuEventArgs* menuEventArgs);
void SaveSettings(Board* mainBoard, char* appVarName);
void LoadSettings(Board* mainBoard, char* appVarName);

#endif