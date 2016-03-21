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

#endif