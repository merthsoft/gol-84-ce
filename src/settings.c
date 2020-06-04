/* Keep these headers */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>
#include <fileioc.h>
#include <string.h>
#include <debug.h>

#include "include/menu.h"
#include "include/key_helper.h"
#include "include/draw.h"

#include "include/settings.h"
#include "include/misc_settings.h"
#include "include/rules_settings.h"
#include "include/topo_settings.h"
#include "include/color_settings.h"
#include "include/board.h"

#include "include/const.h"

const char* GoLMagicString = "GOLV1.2\0";
#define     GolMagicStringLength 8

const char* RulesMagicString = "RULES\0";
#define     RulesMagicStringLength 6

const char* CellsMagicString = "CELLS\0";
#define     CellsMagicStringLength 6

const char* EndMagicString = "END\0";
#define     EndMagicStringLength 6

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

    menu->Tag = board;

    DisplayMenu(menu);
    DeleteMenu(menu);
}

void SaveSettings(Board* mainBoard, const char* appVarName) {
    ti_var_t file;
    Rules* rules;
    uint8_t* cells1;
    uint8_t* cells2;
    uint8_t i, j;

    ti_CloseAll();

    file = ti_Open(appVarName, "w");
    if (!file) { return; }

    rules = mainBoard->Rules;
    cells1 = mainBoard->Cells[mainBoard->BoardNumber];
    cells2 = mainBoard->Cells[!mainBoard->BoardNumber];
    mainBoard->BoardNumber = 0;

    mainBoard->Rules = NULL;
    mainBoard->Cells[0] = NULL;
    mainBoard->Cells[1] = NULL;

    ti_Write(GoLMagicString, GolMagicStringLength, 1, file);
    ti_Write(mainBoard, sizeof(Board), 1, file);
    
    ti_Write(RulesMagicString, RulesMagicStringLength, 1, file);
    ti_Write(&(rules->Born), sizeof(uint16_t), 1, file);
    ti_Write(&(rules->Live), sizeof(uint16_t), 1, file);
    
    ti_Write(CellsMagicString, CellsMagicStringLength, 1, file);
    
    for (i = 0; i <= mainBoard->BoardWidth + 2; i++) {
        ti_Write(cells1, 1, (mainBoard->BoardHeight + 2)*(mainBoard->BoardWidth + 2), file);
    }
    
    ti_Write(EndMagicString, EndMagicStringLength, 1, file);
    
    if (!ti_IsArchived(file)) {
        ti_SetArchiveStatus(true, file);
    }

    ti_CloseAll();

    mainBoard->Rules = rules;
    mainBoard->Cells[0] = cells1;
    mainBoard->Cells[1] = cells2;
}

Board* LoadSettings(const char* appVarName, RulesList* rulesList) {
    ti_var_t file;
    bool found;
    Board* mainBoard;
    uint8_t i, j;

    size_t count = 0;
    char magicStringBuffer[10];

    ti_CloseAll();
    
    file = ti_Open(appVarName, "r+");
    if (!file) { 
        return NULL;  
    }

    mainBoard = malloc(sizeof(Board));
    memset(mainBoard, 0, sizeof(Board));

    if (ti_Read(&magicStringBuffer, GolMagicStringLength, 1, file) != 1)
        goto load_error;

    if (strncmp(magicStringBuffer, GoLMagicString, GolMagicStringLength) != 0)
        goto load_error;

    if (ti_Read(mainBoard, sizeof(Board), 1, file) != 1)
        goto load_error;
    
    mainBoard->Rules = malloc(sizeof(Rules));
    ResizeBoard(mainBoard, mainBoard->BoardWidth, mainBoard->BoardHeight);

    if (ti_Read(magicStringBuffer, RulesMagicStringLength, 1, file) != 1)
        goto load_error;

    if (strncmp(magicStringBuffer, RulesMagicString, RulesMagicStringLength) != 0)
        goto load_error;        

    if (ti_Read(&(mainBoard->Rules->Born), sizeof(uint16_t), 1, file) != 1)
        goto load_error;
    
    if (ti_Read(&(mainBoard->Rules->Live), sizeof(uint16_t), 1, file) != 1)
        goto load_error;

    found = false;
    for (i = 0; i < rulesList->NumRules; i++) {
        if (rulesList->List[i].Born == mainBoard->Rules->Born 
         && rulesList->List[i].Live == mainBoard->Rules->Live) {
            found = true;
            mainBoard->Rules->Name = rulesList->List[i].Name;
            mainBoard->Rules->NumStamps = rulesList->List[i].NumStamps;
            mainBoard->Rules->Stamps = rulesList->List[i].Stamps;
         }
    }

    if (!found) {
        mainBoard->Rules->Name = CustomString;
        mainBoard->Rules->NumStamps = 0;
        mainBoard->Rules->Stamps = NULL;
    }

    if (ti_Read(magicStringBuffer, CellsMagicStringLength, 1, file) != 1)
        goto load_error;

    if (strncmp(magicStringBuffer, CellsMagicString, CellsMagicStringLength) != 0)
        goto load_error;
    
    for (i = 0; i < mainBoard->BoardWidth + 2; i++) {
        ti_Read(mainBoard->Cells[0], 1, (mainBoard->BoardHeight + 2) * (mainBoard->BoardWidth + 2), file);
    }

    ti_CloseAll();
    
    return mainBoard;

load_error:
    DeleteBoard(mainBoard);
    ti_CloseAll();
    return NULL;
}