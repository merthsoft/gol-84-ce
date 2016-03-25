#ifndef SETTINGS_PRIVATE_H
#define SETTINGS_PRIVATE_H

#include "menu.h"

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
void CustomRuleSettings(MenuEventArgs* menuEventArgs);
void CustomRuleMenuItemStrings(Board* mainBoard, Menu* menu);
void SetCustomRule(MenuEventArgs* menuEventArgs);
void SetRuleIndex(MenuEventArgs* menuEventArgs);

#endif