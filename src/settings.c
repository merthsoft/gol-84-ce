/* Keep these headers */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>
#include <fileioc.h>

#include "include/menu.h"
#include "include/key_helper.h"
#include "include/draw.h"
#include "include/rule.h"

#include "include/settings.h"
#include "include/misc_settings.h"
#include "include/rules_settings.h"
#include "include/topo_settings.h"
#include "include/color_settings.h"
#include "include/board.h"

#include "include/const.h"

bool Settings(Board* board, RulesList* rulesList) {
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

    return true;
}

void SaveSettings(Board* mainBoard, char* appVarName) {
    ti_var_t file;
    ti_CloseAll();

    file = ti_Open(appVarName, "w");
    if (!file) { return; }

    ti_Write("Settings:", 9, 1, file);
    ti_Write(&(mainBoard->BoardWidth), 1, 1, file);
    ti_Write(&(mainBoard->BoardHeight), 1, 1, file);
    ti_Write(&(mainBoard->CellWidth), 1, 1, file);
    ti_Write(&(mainBoard->CellHeight), 1, 1, file);
    ti_Write(&(mainBoard->WrappingMode), 3, 1, file);
    ti_Write(&(mainBoard->GridColor), 1, 1, file);
    ti_Write(&(mainBoard->DeadColor), 1, 1, file);
    ti_Write(&(mainBoard->AliveColor), 1, 1, file);
    ti_Write(&(mainBoard->CursorAliveColor), 1, 1, file);
    ti_Write(&(mainBoard->CursorDeadColor), 1, 1, file);
    ti_Write(&(mainBoard->RandomChance), 1, 1, file);
    ti_Write(&(mainBoard->Rule->Born), 1, 1, file);
    ti_Write(&(mainBoard->Rule->Live), 1, 1, file);
    
    if (!ti_IsArchived(file)) {
        ti_SetArchiveStatus(true, file);
    }

    ti_Close(file);
    ti_CloseAll();
}

Board* LoadSettings(char* appVarName) {
    ti_var_t file;
    Board* mainBoard;
    size_t count = 0;

    ti_CloseAll();

    file = ti_Open(appVarName, "r+");
    if (!file) { return NULL;  }

    mainBoard = malloc(sizeof(Board));

    mainBoard->Rule = malloc(sizeof(Board));

    ti_Seek(9, 2, file);

    count = ti_Read(&(mainBoard->BoardWidth), 1, 1, file);
    if (!count) { goto load_error; }

    count = ti_Read(&(mainBoard->BoardHeight), 1, 1, file);
    if (!count) { goto load_error; }

    count = ti_Read(&(mainBoard->CellWidth), 1, 1, file);
    if (!count) { goto load_error; }

    count = ti_Read(&(mainBoard->CellHeight), 1, 1, file);
    if (!count) { goto load_error; }

    count = ti_Read(&(mainBoard->WrappingMode), 3, 1, file);
    if (!count) { goto load_error; }

    count = ti_Read(&(mainBoard->GridColor), 1, 1, file);
    if (!count) { goto load_error; }

    count = ti_Read(&(mainBoard->DeadColor), 1, 1, file);
    if (!count) { goto load_error; }

    count = ti_Read(&(mainBoard->AliveColor), 1, 1, file);
    if (!count) { goto load_error; }

    count = ti_Read(&(mainBoard->CursorAliveColor), 1, 1, file);
    if (!count) { goto load_error; }

    count = ti_Read(&(mainBoard->CursorDeadColor), 1, 1, file);
    if (!count) { goto load_error; }

    count = ti_Read(&(mainBoard->RandomChance), 1, 1, file);
    if (!count) { goto load_error; }
    
    count = ti_Read(&(mainBoard->Rule->Born), 1, 1, file);
    if (!count) { goto load_error; }

    count = ti_Read(&(mainBoard->Rule->Live), 1, 1, file);
    if (!count) {
        load_error:
        free(mainBoard->Rule);
        free(mainBoard);
        return NULL;
    }

    mainBoard->Cells[0] = NULL;
    mainBoard->Cells[1] = NULL;

    ResizeBoard(mainBoard, mainBoard->BoardWidth, mainBoard->BoardHeight);
    mainBoard->Rule->Name = NULL;
    mainBoard->BoardNumber = 0;

    ti_Close(file);
    ti_CloseAll();
    
    return mainBoard;
}