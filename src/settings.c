#include <string.h>
#include <fileioc.h>

#include "include/menu.h"
#include "include/draw.h"

#include "include/settings.h"
#include "include/misc_settings.h"
#include "include/rules_settings.h"
#include "include/topo_settings.h"
#include "include/color_settings.h"
#include "include/board.h"

#include "include/const.h"

const char* GoLMagicString = "GOLV1.4";
#define     GolMagicStringLength 8

const char* CellsMagicString = "CELLS";
#define     CellsMagicStringLength 6

void Settings(Board* board, RulesList* rulesList) {
    Menu* menu = CreateMenu(6, "Settings");
    menu->FillColor = board->DeadColor;

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

    menu->Items[5].Name = (char*)BackString;
    menu->Items[5].Function = FUNCTION_BACK;

    menu->Tag = board;

    DisplayMenu(menu);
    DeleteMenu(menu);
}

typedef struct {
    uint8_t BoardWidth;
    uint8_t BoardHeight;
    uint8_t BoardNumber;
    WrappingMode WrappingMode;
    color GridColor;
    color DeadColor;
    color AliveColor;
    color CursorDeadColor;
    color CursorAliveColor;
    uint8_t CellWidth;
    uint8_t CellHeight;
    uint8_t RandomChance;
    uint8_t CursorX;
    uint8_t CursorY;
    uint8_t OffsetX;
    uint8_t OffsetY;
} SerializedBoard;

void SaveSettings(Board* board, const char* appVarName) {
    ti_var_t file;

    file = ti_Open(appVarName, "w");
    if (!file) { return; }

    ti_Write(GoLMagicString, GolMagicStringLength, 1, file);
    ti_Write(board, sizeof(SerializedBoard), 1, file);
    ti_Write(&(board->Rules.Born), sizeof(uint16_t), 1, file);
    ti_Write(&(board->Rules.Live), sizeof(uint16_t), 1, file);
    ti_Write(CellsMagicString, CellsMagicStringLength, 1, file);
    
    ti_Write(board->Cells[board->BoardNumber], 1, (board->BoardHeight + 2)*(board->BoardWidth + 2), file);

    if (!ti_IsArchived(file)) {
        ti_SetArchiveStatus(true, file);
    }

    ti_Close(file);
}

void LoadSettings(Board* board, const char* appVarName, RulesList* rulesList) {
    ti_var_t file;
    SerializedBoard serializedBoard;
    bool found;
    uint8_t i;

    char magicStringBuffer[10];

    memset(board, 0, sizeof(Board));
    
    file = ti_Open(appVarName, "r+");
    if (!file) { 
        return;  
    }

    if (ti_Read(&magicStringBuffer, GolMagicStringLength, 1, file) != 1)
        goto load_error;

    if (strncmp(magicStringBuffer, GoLMagicString, GolMagicStringLength) != 0)
        goto load_error;

    if (ti_Read(&serializedBoard, sizeof(SerializedBoard), 1, file) != 1)
        goto load_error;

    InitializeBoard(board, serializedBoard.BoardWidth, serializedBoard.BoardHeight);
    memcpy(board, &serializedBoard, sizeof(SerializedBoard));

    if (ti_Read(&(board->Rules.Born), sizeof(uint16_t), 1, file) != 1)
        goto load_error;
    
    if (ti_Read(&(board->Rules.Live), sizeof(uint16_t), 1, file) != 1)
        goto load_error;

    found = false;
    for (i = 0; i < rulesList->NumRules; i++) {
        if (rulesList->List[i].Born == board->Rules.Born 
         && rulesList->List[i].Live == board->Rules.Live) {
            found = true;
            board->Rules.Name = rulesList->List[i].Name;
            board->Rules.NumStamps = rulesList->List[i].NumStamps;
            board->Rules.Stamps = rulesList->List[i].Stamps;
            break;
         }
    }

    if (!found) {
        board->Rules.Name = (char*)CustomString;
        board->Rules.NumStamps = 0;
        board->Rules.Stamps = NULL;
    }

    if (ti_Read(magicStringBuffer, CellsMagicStringLength, 1, file) != 1)
        goto load_error;

    if (strncmp(magicStringBuffer, CellsMagicString, CellsMagicStringLength) != 0)
        goto load_error;
    
    ti_Read(board->Cells[board->BoardNumber], 1, (board->BoardHeight + 2) * (board->BoardWidth + 2), file);
    memcpy(board->Cells[!board->BoardNumber], board->Cells[board->BoardNumber], (board->BoardHeight + 2) * (board->BoardWidth + 2));
    ti_Close(file);
    board->IsInitialized = true;
    
    return;

load_error:
    DeleteBoard(board);
    ti_Close(file);
    return;
}