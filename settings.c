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
#include "draw.h"
#include "rule.h"

#include "settings.h"
#include "misc_settings.h"
#include "rules_settings.h"
#include "topo_settings.h"
#include "color_settings.h"
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