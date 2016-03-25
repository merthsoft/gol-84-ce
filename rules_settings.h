#ifndef RULES_SETTINGS_H
#define RULES_SETTINGS_H


#include "menu.h"

void CustomRuleSettings(MenuEventArgs* menuEventArgs);
void CustomRuleMenuItemStrings(Board* mainBoard, Menu* menu);
void SetCustomRule(MenuEventArgs* menuEventArgs);
void SetRuleIndex(MenuEventArgs* menuEventArgs);
void RuleSettings(MenuEventArgs* menuEventArgs);
void SetRuleMenuEvent(MenuEventArgs* menuEventArgs);

#endif