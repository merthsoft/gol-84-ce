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

#include <lib/ce/fileioc.h>
#include <lib/ce/graphc.h>

#include "menu.h"
#include "key_helper.h"
#include "board.h"
#include "draw.h"
#include "rule.h"
#include "main.h"
#include "topo_sprites.h"

#include "settings.h"
#include "settings_private.h"
#include "board.h"

void Settings(Board* board, RulesList* rulesList) {
    Menu* menu = CreateMenu(6, "Settings");

    menu->Items[0].Name = "Colors...";
    menu->Items[0].Function = ColorSettings;

    menu->Items[1].Name = "Rules...";
    menu->Items[1].Function = RuleSettings;
    menu->Items[1].Tag = rulesList; 

    menu->Items[2].Name = "Topologies...";
    menu->Items[2].Function = TopologySettings;

    menu->Items[3].Name = "Cell Size...";
    menu->Items[3].Function = CellSizeSettings;

    menu->Items[4].Name = "Random Percentage...";
    menu->Items[4].Function = RandomPercentageSettings;

    menu->Items[5].Name = BackString;
    menu->Items[5].Function = FUNCTION_BACK;

    menu->BackKey = Key_Del;
    menu->Tag = board;

    DisplayMenu(menu);
    DeleteMenu(menu);
}

void CellSizeSettings(MenuEventArgs* menuEventArgs) {
    uint8_t i;
    uint8_t previous;
    Board* mainBoard = menuEventArgs->Menu->Tag;

    Menu* menu = CreateMenu(16, "Cell Size");
    menu->SelectionType = Single;

    menu->Items[0].Name = "1";
    menu->Items[1].Name = "2";
    menu->Items[2].Name = "3";
    menu->Items[3].Name = "4";
    menu->Items[4].Name = "5";
    menu->Items[5].Name = "6";
    menu->Items[6].Name = "7";
    menu->Items[7].Name = "8";
    menu->Items[8].Name = "9";
    menu->Items[9].Name = "10";
    menu->Items[10].Name = "11";
    menu->Items[11].Name = "12";
    menu->Items[12].Name = "13";
    menu->Items[13].Name = "14";
    menu->Items[14].Name = "15";
    menu->Items[15].Name = BackString;
    menu->Items[15].Function = FUNCTION_BACK;

    menu->Items[mainBoard->CellHeight - 1].Selected = true;
    previous = mainBoard->CellHeight - 1;

    menu->BackKey = Key_Del;
    DisplayMenu(menu);

    for (i = 0; i < menu->NumItems - 1; i++) {
        if (menu->Items[i].Selected && i != previous) {
            int newSize = 224 / (i + 1);
            if (newSize > 80) { newSize = 80; }

            ResizeBoard(mainBoard, newSize, newSize);

            mainBoard->CellHeight = i + 1;
            mainBoard->CellWidth = i + 1;
            break;
        }
    }

    DeleteMenu(menu);
}

void RandomPercentageSettings(MenuEventArgs* menuEventArgs) {
    uint8_t i;
    uint8_t previous;
    Board* mainBoard = menuEventArgs->Menu->Tag;

    Menu* menu = CreateMenu(10, "Cell Size");
    menu->SelectionType = Single;

    menu->Items[0].Name = "10";
    menu->Items[1].Name = "20";
    menu->Items[2].Name = "30";
    menu->Items[3].Name = "40";
    menu->Items[4].Name = "50";
    menu->Items[5].Name = "60";
    menu->Items[6].Name = "70";
    menu->Items[7].Name = "80";
    menu->Items[8].Name = "90";;
    menu->Items[9].Name = BackString;
    menu->Items[9].Function = FUNCTION_BACK;

    menu->Items[(mainBoard->RandomChance - 1) / 10].Selected = true;

    menu->BackKey = Key_Del;
    DisplayMenu(menu);

    for (i = 0; i < 9; i++) {
        if (menu->Items[i].Selected) {
            mainBoard->RandomChance = (i + 1) * 10;
            break;
        }
    }

    DeleteMenu(menu);
}

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

void ColorSettings(MenuEventArgs* menuEventArgs) {
    uint8_t i;
    Board* mainBoard = menuEventArgs->Menu->Tag;
    Menu* menu;

    Board* sampleBoard = CreateBoard(5, 5);
    sprintf(dbgout, "Sample board location: %p\n", sampleBoard);
    sampleBoard->CellHeight = mainBoard->CellHeight;
    sampleBoard->CellWidth = mainBoard->CellWidth;

    sampleBoard->Cells[0][3][2] = 1;
    sampleBoard->Cells[0][4][3] = 1;
    sampleBoard->Cells[0][2][4] = 1;
    sampleBoard->Cells[0][3][4] = 1;
    sampleBoard->Cells[0][4][4] = 1;
    sampleBoard->Cells[0][2][1] = 1;
    sampleBoard->Cells[1][4][4] = 1;
    sampleBoard->Cells[1][2][1] = 1;

    sampleBoard->GridColor = mainBoard->GridColor;
    sampleBoard->DeadColor = mainBoard->DeadColor;
    sampleBoard->AliveColor = mainBoard->AliveColor;
    sampleBoard->CursorDeadColor = mainBoard->CursorDeadColor;
    sampleBoard->CursorAliveColor = mainBoard->CursorAliveColor;

    menu = CreateMenu(7, "Colors");

    menu->ExtraFunction = DrawSampleBoard;

    menu->Items[0].Name = "Grid...";
    menu->Items[1].Name = "Dead...";
    menu->Items[2].Name = "Alive...";
    menu->Items[3].Name = "Cursor (Dead)...";
    menu->Items[4].Name = "Cursor (Alive)...";
    menu->Items[5].Name = "Save";
    menu->Items[5].Function = SaveColors;
    menu->Items[5].Tag = sampleBoard;
    menu->Items[6].Name = BackString;
    menu->Items[6].Function = FUNCTION_BACK;

    for (i = 0; i < 5; i++) {
        menu->Items[i].Function = ColorPicker;
        menu->Items[i].Tag = sampleBoard;
    }

    menu->BackKey = Key_Del;
    menu->Tag = mainBoard;

    DisplayMenu(menu);
    DeleteMenu(menu);
    DeleteBoard(sampleBoard);
}

void __drawSampleBoard(Board* sampleBoard) {
    gc_PrintStringXY("Sample:", 150, 0);
    DrawGrid(sampleBoard, 150, 9);
    DrawBoard(sampleBoard, true, 150, 9);
    DrawCursor(sampleBoard, 1, 1, 150, 9);
    DrawCursor(sampleBoard, 2, 1, 150, 9);
}

void DrawSampleBoard(MenuEventArgs* menuEventArgs) {
    if (menuEventArgs->FrameNumber == 0) {
        __drawSampleBoard(menuEventArgs->Menu->Items[menuEventArgs->Index].Tag);
    }
}

void ColorPicker(MenuEventArgs* menuEventArgs) {
    uint8_t a = 0;
    uint8_t i, j, c = 0;
    uint8_t old_i, old_j;

    Board* mainBoard = menuEventArgs->Menu->Tag;
    Board* sampleBoard = menuEventArgs->Menu->Items[menuEventArgs->Index].Tag;

    gc_SetColorIndex(0);
    gc_ClipHorizLine(0, 85, 320);
    for (j = 0; j < 8; j++) {
        for (i = 0; i < 32; i++) {
            DrawRectFill(10 * i + 1, 10 * j + 100, 8, 8, c++);
        }
    }
    SetTextColor(0, 255);
    gc_PrintStringXY("Select color for: ", 1, 87);
    gc_PrintStringXY(menuEventArgs->Menu->Items[menuEventArgs->Index].Name, 120, 87);

    switch (menuEventArgs->Index) {
    case 0:
        c = mainBoard->GridColor;
        break;
    case 1:
        c = mainBoard->DeadColor;
        break;
    case 2:
        c = mainBoard->AliveColor;
        break;
    case 3:
        c = mainBoard->CursorDeadColor;
        break;
    case 4:
        c = mainBoard->CursorAliveColor;
        break;
    }

    i = c % 32;
    j = c / 32;

    while (true) {
        gc_SetColorIndex(0);
        gc_NoClipRectangleOutline(10 * i, 10 * j + 99, 10, 10);

        Key_ScanKeys(125);

        old_i = i;
        old_j = j;

        if (Key_IsDown(Key_Up)) { j = j == 0 ? 7 : j - 1; }
        else if (Key_IsDown(Key_Down)) { j = j == 7 ? 0 : j + 1; }
        else if (Key_IsDown(Key_Left)) { i = i == 0 ? 31 : i - 1; }
        else if (Key_IsDown(Key_Right)) { i = i == 31 ? 0 : i + 1; }
        else if (Key_JustPressed(Key_2nd) || Key_JustPressed(Key_Enter)) {
            c = i % 32 + j * 32;

            switch (menuEventArgs->Index) {
            case 0:
                sampleBoard->GridColor = c;
                break;
            case 1:
                sampleBoard->DeadColor = c;
                break;
            case 2:
                sampleBoard->AliveColor = c;
                break;
            case 3:
                sampleBoard->CursorDeadColor = c;
                break;
            case 4:
                sampleBoard->CursorAliveColor = c;
                break;
            }

            __drawSampleBoard(sampleBoard);
        }
        else if (Key_JustPressed(Key_Del)) {
            menuEventArgs->FrameNumber = -1; // Force a re-draw of the sample board
            return;
        }

        if (i != old_i || j != old_j) {
            gc_SetColorIndex(255);
            gc_NoClipRectangleOutline(10 * old_i, 10 * old_j + 99, 10, 10);
        }
    }
}

void SaveColors(MenuEventArgs* menuEventArgs) {
    Board* mainBoard = menuEventArgs->Menu->Tag;
    Board* sampleBoard = menuEventArgs->Menu->Items[menuEventArgs->Index].Tag;

    sprintf(dbgout, "Sample board location: %p\n", sampleBoard);

    mainBoard->GridColor = sampleBoard->GridColor;
    mainBoard->DeadColor = sampleBoard->DeadColor;
    mainBoard->AliveColor = sampleBoard->AliveColor;
    mainBoard->CursorDeadColor = sampleBoard->CursorDeadColor;
    mainBoard->CursorAliveColor = sampleBoard->CursorAliveColor;
    menuEventArgs->Back = true;
}

void TopologySettings(MenuEventArgs* menuEventArgs) {
    int i;
    Board* mainBoard = menuEventArgs->Menu->Tag;

    Menu* menu = CreateMenu(NUM_WRAPPING_MODES + 1, "Topologies");
    menu->SelectionType = Single;
    menu->ExtraFunction = DrawTopoSprite;

    for (i = 0; i < NUM_WRAPPING_MODES; i++) {
        menu->Items[i].Name = WrappingModeNames[i];
    }
    menu->Items[NUM_WRAPPING_MODES].Name = BackString;
    menu->Items[NUM_WRAPPING_MODES].Function = FUNCTION_BACK;

    menu->Items[mainBoard->WrappingMode].Selected = true;

    for (i = 0; i < NUM_WRAPPING_MODES; i++) {
        menu->Items[i].Function = DrawTopoSprite;
    }

    menu->BackKey = Key_Del;
    DisplayMenu(menu);

    for (i = 0; i < NUM_WRAPPING_MODES; i++) {
        if (menu->Items[i].Selected) {
            mainBoard->WrappingMode = i;
            break;
        }
    }

    DeleteMenu(menu);
}

void DrawTopoSprite(MenuEventArgs* menuEventArgs) {
    uint8_t* sprite = NULL;

    switch (menuEventArgs->Index) {
        case 0:
            sprite = topo_plane;
            break;
        case 1:
            sprite = topo_ring;
            break;
        case 2:
            sprite = topo_mobius;
            break;
        case 3:
            sprite = topo_torus;
            break;
        case 4:
            sprite = topo_sphere;
            break;
        case 5:
            sprite = topo_klein;
            break;
        case 6:
            sprite = topo_proj;
            break;
        default:
            sprite = topo_plane;
            break;
    }

    gc_NoClipDrawScaledSprite(sprite, 150, 9, 16, 16, 9, 9);
}

void SaveSettings(Board* mainBoard, char* appVarName) {
    ti_var_t file;
    ti_CloseAll();

    file = ti_Open(appVarName, "w");
    if (!file) { return; }
    if (ti_IsArchived(file)) {
        ti_SetArchiveStatus(false, file);
        file = ti_Open(appVarName, "w");
    }

    FreeCells(mainBoard);
    ti_Write("SETTINGS:", 9, 1, file);
    ti_Write(mainBoard, sizeof(Board), 1, file);
    ti_Write(mainBoard->Rule, sizeof(Rule), 1, file);

    ti_SetArchiveStatus(true, file);
}

void LoadSettings(Board* mainBoard, char* appVarName) {
    ti_var_t file;
    ti_CloseAll();

    file = ti_Open(appVarName, "r");
    if (!file) { return; }
    if (ti_IsArchived(file)) {
        ti_SetArchiveStatus(false, file);
        file = ti_Open(appVarName, "r");
    }

    mainBoard = malloc(sizeof(Board));

    ti_Seek(9, SEEK_SET, file);
    if (!ti_Read(mainBoard, sizeof(Board), 1, file)) {
        free(mainBoard);
        mainBoard = NULL;
        return;
    }

    mainBoard->Rule = malloc(sizeof(Rule));
    if (!ti_Read(mainBoard->Rule, sizeof(Rule), 1, file)) {
        free(mainBoard->Rule);
        free(mainBoard);
        mainBoard = NULL;
        return;
    }

    mainBoard->Cells[0] = NULL;
    mainBoard->Cells[1] = NULL;
    ResizeBoard(mainBoard, mainBoard->BoardWidth, mainBoard->BoardHeight);
    mainBoard->Rule->Name = NULL;

    ti_Close(file);
}