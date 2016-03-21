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

#include "main.h"
#include "key_helper.h"
#include "board.h"
#include "draw.h"
#include "menu.h"
#include "rule.h"
#include "settings.h"

Board* mainBoard;
RulesList* rulesList;

const char appVarName[] = "[GoL";

void main(void) {
    uint8_t x = 1;
    uint8_t y = 1;
    uint8_t old_x = x;
    uint8_t old_y = y;
    uint8_t i;
    
    bool redraw = true;
    bool toggled = false;
    bool running = false;
    bool quit = false;
    
    gc_InitGraph();
    gc_FillScrn(0);
    rulesList = InitRules();

    mainBoard = NULL;
    //LoadSettings();

    if (mainBoard == NULL) {
        mainBoard = CreateBoard(32, 32);
        mainBoard->CellHeight = 7;
        mainBoard->CellWidth = 7;

        mainBoard->AliveColor = 18;
        mainBoard->DeadColor = 255;
        mainBoard->GridColor = 0;
        mainBoard->WrappingMode = Torus;
        mainBoard->CursorDeadColor = 224;
        mainBoard->CursorAliveColor = 15;
        mainBoard->RandomMod = 2;
    }
    
    ClearBoard(mainBoard);
    SetRule(mainBoard, rulesList->List);
    Key_Init();

    while (!quit) {
        if (redraw) {
            uint8_t i = 0;
            char* rulesString = malloc(sizeof(char) * 11);
            memset(rulesString, 0, 11);
                        
            gc_FillScrn(255);
            gc_PrintStringXY("2nd-Toggle", 240, 8);
            gc_PrintStringXY("Del-Quit", 240, 17);
            gc_PrintStringXY("Clear-Clear", 240, 26);
            gc_PrintStringXY("Vars-Rand", 240, 35);
            gc_PrintStringXY("+-Step", 240, 44);
            gc_PrintStringXY("Mode-Setup", 240, 53);
            gc_PrintStringXY("Enter-", 240, 62);
            DrawGrid(mainBoard, 0, 0);
            DrawBoard(mainBoard, true, 0, 0);     
            DrawPlayPauseIcon(running);

            DrawRectFill(239, 72, 80, 2, 0);

            gc_PrintStringXY("Topology:", 240, 77);
            gc_PrintStringXY(WrappingModeNames[mainBoard->WrappingMode], 245, 86);
            gc_PrintStringXY("Rules:", 240, 95);
            
            rulesString[0] = 'B';
            i = 1;
            NumToRuleString(mainBoard->Rule->Born, rulesString, &i);
            gc_PrintStringXY(rulesString, 245, 104);

            memset(rulesString, 0, 11);
            rulesString[0] = 'S';
            i = 1;
            NumToRuleString(mainBoard->Rule->Live, rulesString, &i);
            gc_PrintStringXY(rulesString, 245, 113);

            gc_PrintStringXY("Cell Size:", 240, 122);
            gc_SetTextXY(245, 131);
            gc_PrintInt(mainBoard->CellHeight, log10(mainBoard->CellHeight) + 1);

            free(rulesString);
            redraw = false;
        }
        
        if (running) {
            Step(mainBoard);

            DrawBoard(mainBoard, false, 0, 0);
            
            Key_ScanKeys(0);
            if (Key_JustPressed(Key_Enter)) {
                running = false;
                DrawPlayPauseIcon(running);
            }
            else if (Key_JustPressed(Key_Del)) { quit = true; }
        } else {
            DrawCursor(mainBoard, x, y, 0, 0);

            Key_ScanKeys(125);

            old_x = x;
            old_y = y;

            if (Key_IsDown(Key_Up)) { y = y == 1 ? mainBoard->BoardHeight : y - 1; }
            else if (Key_IsDown(Key_Down)) { y = y == mainBoard->BoardHeight ? 1 : y + 1; }
            else if (Key_IsDown(Key_Left)) { x = x == 1 ? mainBoard->BoardWidth : x - 1; }
            else if (Key_IsDown(Key_Right)) { x = x == mainBoard->BoardWidth ? 1 : x + 1; }
            else if (Key_JustPressed(Key_Enter)) { 
                running = true;
                DrawPlayPauseIcon(running);
            }
            else if (Key_IsDown(Key_Clear)) { 
                ClearBoard(mainBoard);
                DrawBoard(mainBoard, true, 0, 0);
            } else if (Key_IsDown(Key_Mode)) {
                Settings(mainBoard, rulesList);
                redraw = true;
            } else if (Key_IsDown(Key_Add)) {
                Step(mainBoard);
                DrawBoard(mainBoard, false, 0, 0);
            } else if (Key_IsDown(Key_Del)) { quit = true; } 
            else if (Key_IsDown(Key_Vars)) {
                RandomBoard(mainBoard);
                redraw = true;
            }

            if (Key_JustPressed(Key_2nd)) {
                mainBoard->Cells[mainBoard->BoardNumber][x][y] = !mainBoard->Cells[mainBoard->BoardNumber][x][y];
                toggled = true;
            }

            if (old_x != x || old_y != y || running) {
                DrawCell(mainBoard, old_x, old_y, 0, 0);
            }
        }
    }
    
    SaveSettings();
    //DeleteBoard(mainBoard);

    Key_Reset();
    gc_CloseGraph();
    pgrm_CleanUp();
}

void SaveSettings() {
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

void LoadSettings() {
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

void DrawPlayPauseIcon(bool running) {
    if (!running) {
        DrawRectFill(282, 61, 7, 11, 255);
        gc_SetColorIndex(4);
        gc_NoClipVertLine(282, 61, 11);
        gc_NoClipVertLine(283, 62, 9);
        gc_NoClipVertLine(284, 63, 7);
        gc_NoClipVertLine(285, 64, 5);
        gc_NoClipVertLine(286, 65, 3);
        gc_NoClipVertLine(287, 66, 1);
    }
    else {
        DrawRectFill(282, 61, 7, 11, 255);
        DrawRectFill(282, 61, 3, 9, 128);
        DrawRectFill(286, 61, 3, 9, 128);
    }
}

void NumToRuleString(uint16_t num, char* rulesString, uint8_t* index) {
    uint8_t j;
    for (j = 0; j < 9; j++) {
        if ((num & (1 << j)) != 0) {
            rulesString[*index] = 48 + j;
            (*index) = (*index) + 1;
        }
    }
}