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

#include "menu.h"
#include "key_helper.h"
#include "board.h"
#include "rule.h"

#include "rules_settings.h"

void RuleSettings(MenuEventArgs* menuEventArgs) {
    int i;
    bool foundRule = false;
    Board* mainBoard = menuEventArgs->Menu->Tag;
    RulesList* list = menuEventArgs->Menu->Items[menuEventArgs->Index].Tag;
    uint8_t numRules = list->NumRules;
    Rule* rulesList = list->List;

    Menu* menu = CreateMenu(numRules + 2, "Rules");
    menu->SelectionType = Single;

    for (i = 0; i < numRules; i++) {
        menu->Items[i].Name = rulesList[i].Name;
        menu->Items[i].Function = SetRuleMenuEvent;
        menu->Items[i].Tag = list;
        if (rulesList[i].Born == mainBoard->Rule->Born && rulesList[i].Live == mainBoard->Rule->Live) {
            menu->Items[i].Selected = true;
            foundRule = true;
        }
    }

    menu->Items[numRules].Name = "Custom...";
    menu->Items[numRules].Function = CustomRuleSettings;
    menu->Items[numRules].Selected = !foundRule;

    menu->Items[numRules + 1].Name = BackString;
    menu->Items[numRules + 1].Function = FUNCTION_BACK;

    menu->BackKey = Key_Del;
    menu->Tag = mainBoard;

    DisplayMenu(menu);
    DeleteMenu(menu);
}

void CustomRuleSettings(MenuEventArgs* menuEventArgs) {
    Board* mainBoard = menuEventArgs->Menu->Tag;
    Menu* menu = CreateMenu(3, "Custom Rule");

    menu->Items[0].Name = malloc(20);
    menu->Items[1].Name = malloc(17);

    CustomRuleMenuItemStrings(mainBoard, menu);

    menu->Items[0].Function = SetCustomRule;
    menu->Items[1].Function = SetCustomRule;

    menu->Items[2].Name = BackString;
    menu->Items[2].Function = FUNCTION_BACK;

    menu->Tag = mainBoard;
    menu->BackKey = Key_Del;

    DisplayMenu(menu);

    free(menu->Items[0].Name);
    free(menu->Items[1].Name);
    DeleteMenu(menu);
}

void CustomRuleMenuItemStrings(Board* mainBoard, Menu* menu) {
    uint8_t i;

    memcpy(menu->Items[0].Name, "Survive: \0\0\0\0\0\0\0\0\0\0\0", 20);
    memcpy(menu->Items[1].Name, "Born: \0\0\0\0\0\0\0\0\0\0\0", 17);

    i = 9;
    NumToRuleString(mainBoard->Rule->Live, menu->Items[0].Name, &i);
    i = 6;
    NumToRuleString(mainBoard->Rule->Born, menu->Items[1].Name, &i);
}

void SetCustomRule(MenuEventArgs* menuEventArgs) {
    Menu* menu;
    uint8_t i;
    Board* mainBoard = menuEventArgs->Menu->Tag;
    char* ruleName;
    int* rule;

    if (menuEventArgs->Index == 0) {
        ruleName = "Survive";
        rule = &(mainBoard->Rule->Live);
    } else {
        ruleName = "Born";
        rule = &(mainBoard->Rule->Born);
    }

    menu = CreateMenu(10, ruleName);
    menu->SelectionType = Multiple;

    for (i = 0; i < 9; i++) {
        menu->Items[i].Name = malloc(2);
        menu->Items[i].Name[0] = 48 + i;
        menu->Items[i].Name[1] = 0;
        menu->Items[i].Selected = ((*rule) & (1 << i)) != 0;
        menu->Items[i].Tag = rule;
        menu->Items[i].Function = SetRuleIndex;
    }

    menu->Items[9].Name = BackString;
    menu->Items[9].Function = FUNCTION_BACK;

    menu->Tag = mainBoard;
    menu->BackKey = Key_Del;
    DisplayMenu(menu);

    for (i = 0; i < 9; i++) {
        free(menu->Items[i].Name);
    }

    CustomRuleMenuItemStrings(mainBoard, menuEventArgs->Menu);

    DeleteMenu(menu);
}

void SetRuleIndex(MenuEventArgs* menuEventArgs) {
    uint8_t index = menuEventArgs->Index;
    MenuItem item = menuEventArgs->Menu->Items[index];
    int* rule = menuEventArgs->Menu->Items[index].Tag;
    *rule ^= 1 << index;
}

void SetRuleMenuEvent(MenuEventArgs* menuEventArgs) {
    Board* mainBoard = menuEventArgs->Menu->Tag;
    RulesList* list = menuEventArgs->Menu->Items[menuEventArgs->Index].Tag;
    SetRule(mainBoard, &list->List[menuEventArgs->Index]);
}