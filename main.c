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

#include <lib/ce/graphc.h>

#include "main.h"
#include "key_helper.h"
#include "board.h"
#include "draw.h"
#include "menu.h"
#include "rule.h"
#include "settings.h"

void main(void) {
    uint8_t x = 1;
    uint8_t y = 1;
    uint8_t old_x = x;
    uint8_t old_y = y;
    uint8_t i;

    Board* mainBoard;
    RulesList* rulesList;
    const char appVarName[] = "[GoL";
    
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
        mainBoard->RandomChance = 50;
    }
    
    ClearBoard(mainBoard);
    SetRule(mainBoard, rulesList->List);
    Key_Init();

    while (!quit) {
        if (redraw) {
            DrawHelpText(mainBoard, running);
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
            } else if (Key_IsDown(Key_Clear)) {
                ClearBoard(mainBoard);
                DrawBoard(mainBoard, true, 0, 0);
            } else if (Key_IsDown(Key_Power)) {
                FillBoard(mainBoard);
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
    
    SaveSettings(mainBoard, appVarName);
    //DeleteBoard(mainBoard);

    Key_Reset();
    gc_CloseGraph();
    pgrm_CleanUp();
}

void DrawHelpText(Board* mainBoard, bool running) {
    uint8_t i = 0;
    char* rulesString = malloc(sizeof(char) * 11);
    memset(rulesString, 0, 11);

    gc_FillScrn(255);
    gc_PrintStringXY("2nd-Toggle", 240, 8);
    gc_PrintStringXY("Del-Quit", 240, 17);
    gc_PrintStringXY("Clear-Clear", 240, 26);
    gc_PrintStringXY("^-Fill", 240, 35);
    gc_PrintStringXY("Vars-Rand", 240, 44);
    gc_PrintStringXY("+-Step", 240, 53);
    gc_PrintStringXY("Mode-Setup", 240, 62);
    gc_PrintStringXY("Enter-", 240, 71);
    DrawGrid(mainBoard, 0, 0);
    DrawBoard(mainBoard, true, 0, 0);
    DrawPlayPauseIcon(running);

    DrawRectFill(239, 81, 80, 2, 0);

    gc_PrintStringXY("Topology:", 240, 86);
    gc_PrintStringXY(WrappingModeNames[mainBoard->WrappingMode], 243, 95);
    gc_PrintStringXY("Rules:", 240, 104);

    gc_PrintStringXY("Stay alive:", 245, 113);
    memset(rulesString, 0, 11);
    i = 0;
    NumToRuleString(mainBoard->Rule->Live, rulesString, &i);
    gc_PrintStringXY(rulesString, 245, 122);

    gc_PrintStringXY("Born:", 245, 131);
    memset(rulesString, 0, 11);
    i = 0;
    NumToRuleString(mainBoard->Rule->Born, rulesString, &i);
    gc_PrintStringXY(rulesString, 245, 131);

    gc_PrintStringXY("Cell Size:", 240, 140);
    sprintf(rulesString, "%d", mainBoard->CellHeight);
    gc_PrintStringXY(rulesString, 245, 149);

    free(rulesString);
}

void DrawPlayPauseIcon(bool running) {
    #define COLUMN 287
    #define ROW 70
    DrawRectFill(COLUMN, ROW, 7, 11, 255);
    if (!running) {
        gc_SetColorIndex(4);
        gc_NoClipVertLine(COLUMN, ROW, 11);
        gc_NoClipVertLine(COLUMN+1, ROW+1, 9);
        gc_NoClipVertLine(COLUMN+2, ROW+2, 7);
        gc_NoClipVertLine(COLUMN+3, ROW+3, 5);
        gc_NoClipVertLine(COLUMN+4, ROW+4, 3);
        gc_NoClipVertLine(COLUMN+5, ROW+5, 1);
    }
    else {
        DrawRectFill(COLUMN, ROW, 3, 9, 128);
        DrawRectFill(COLUMN+4, ROW, 3, 9, 128);
    }
}