#include <debug.h>
#include <tice.h>
#include <graphx.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "include/key_helper.h"
#include "include/board.h"
#include "include/draw.h"
#include "include/menu.h"
#include "include/rules.h"
#include "include/settings.h"
#include "include/const.h"
#include "include/stamp_picker.h"
#include "include/stamp.h"

void DrawPlayPauseIcon(bool running);
void DrawHelpText(Board* mainBoard, bool running);

const char appVarName[] = "[GoL";

int main(void) {
    uint8_t x, old_x;
    uint8_t y, old_y;
    Board* mainBoard;
    RulesList* rulesList;
    Stamp* selectedStamp = NULL;
    
    bool redraw = true;
    bool running = false;
    bool quit = false;
    
    rulesList = InitRules();

    mainBoard = NULL;
    mainBoard = LoadSettings(appVarName, rulesList);
    
    if (mainBoard == NULL) {
        mainBoard = CreateBoard(0, 0);
        SquareCells(mainBoard, 10, MAX_BOARD_DRAW_SIZE, MAX_BOARD_SIZE);

        mainBoard->AliveColor = 18;
        mainBoard->DeadColor = 255;
        mainBoard->GridColor = 0;
        mainBoard->CursorDeadColor = 224;
        mainBoard->CursorAliveColor = 15;
        
        mainBoard->WrappingMode = Torus;
        mainBoard->RandomChance = 50;

        SetRules(mainBoard, rulesList->List);
    }
    
    Key_Init();
    
    GraphicsBegin();
    ClearScreen();

    while (!quit) {
        if (redraw) {
            DrawHelpText(mainBoard, running);
            redraw = false;
        }
        
        if (running) {
            Step(mainBoard);
            
            Key_ScanKeys(0);
            if (Key_JustPressed(Key_Enter)) 
            {
                running = false;
                DrawPlayPauseIcon(running);
            }
            else if (Key_JustPressed(Key_Del)) 
                quit = true;
        } else {
            DrawCursor(mainBoard, selectedStamp, false);
                       
            x = old_x = mainBoard->CursorX;
            y = old_y = mainBoard->CursorY;

            Key_ScanKeys(150);
            if (Key_IsDown(Key_Up)) 
                y = y == 1 ? mainBoard->BoardHeight : y - 1;
            else if (Key_IsDown(Key_Down)) 
                y = y == mainBoard->BoardHeight ? 1 : y + 1;
            else if (Key_IsDown(Key_Left))
                x = x == 1 ? mainBoard->BoardWidth : x - 1;
            else if (Key_IsDown(Key_Right)) 
                x = x == mainBoard->BoardWidth ? 1 : x + 1;
            else if (Key_IsDown(Key_Del)) 
                quit = selectedStamp == NULL ? true : (selectedStamp = NULL) == NULL;
            else if (Key_IsDown(Key_Add))
                Step(mainBoard);
            else if (Key_IsDown(Key_Mode)) {
                Settings(mainBoard, rulesList);
                redraw = true;
            } else if (Key_IsDown(Key_Vars)) {
                RandomBoard(mainBoard);
                redraw = true;
            } else if (Key_JustPressed(Key_Enter)) {
                DrawPlayPauseIcon(true);
                running = true;
            } else if (Key_JustPressed(Key_Alpha)){ 
                selectedStamp = ChooseStamp(mainBoard);
                if (selectedStamp == NULL)
                    dbg_sprintf(dbgout, "Stamp NULL\n");
                else
                    dbg_sprintf(dbgout, "Stamp %.20s\n", selectedStamp->Name);
                redraw = true;
            } else if (Key_IsDown(Key_Clear)) {
                ClearBoard(mainBoard);
                DrawBoard(mainBoard, true);
            } else if (Key_IsDown(Key_Power)) {
                FillBoard(mainBoard);
                DrawBoard(mainBoard, true);
            } else if (Key_JustPressed(Key_2nd)) {
                if (selectedStamp == NULL) {
                    ToggleCell(mainBoard);
                } else {
                    PlaceStamp(mainBoard, selectedStamp);
                }
            }

            if (old_x != x || old_y != y || running) {
                DrawCursor(mainBoard, selectedStamp, true);
            }

            mainBoard->CursorX = x;
            mainBoard->CursorY = y;
        }
    }
    
    SaveSettings(mainBoard, appVarName);
    DeleteBoard(mainBoard);

    Key_Reset();
    GraphicsEnd();

    return 0;
}

void DrawHelpText(Board* mainBoard, bool running) {
    uint8_t i = 0;
    char* rulesString = malloc(sizeof(char) * 11);
    memset(rulesString, 0, 11);

    FillScreen(255);
    DrawString("2nd-Toggle", HELP_TEXT_X, 8);
    DrawString("Del-Quit", HELP_TEXT_X, 17);
    DrawString("Clear-Clear", HELP_TEXT_X, 26);
    DrawString("^-Fill", HELP_TEXT_X, 35);
    DrawString("Vars-Rand", HELP_TEXT_X, 44);
    DrawString("+-Step", HELP_TEXT_X, 53);
    DrawString("Mode-Setup", HELP_TEXT_X, 62);
    DrawString("Enter-", HELP_TEXT_X, 71);
    DrawGrid(mainBoard);
    DrawBoard(mainBoard, true);
    DrawPlayPauseIcon(running);

    DrawRectFill(239, 81, 80, 2, 0);

    DrawString("Topology:", HELP_TEXT_X, 86);
    DrawString(WrappingModeNames[mainBoard->WrappingMode], HELP_TEXT_X + 3, 95);
    DrawString("Rules:", HELP_TEXT_X, 104);

    DrawString("Stay alive:", HELP_TEXT_X + 5, 113);
    memset(rulesString, 0, 11);
    i = 0;
    NumToRuleString(mainBoard->Rules->Live, rulesString, &i);
    DrawString(rulesString, HELP_TEXT_X + 5, 122);

    DrawString("Born:", HELP_TEXT_X + 5, 131);
    memset(rulesString, 0, 11);
    i = 0;
    NumToRuleString(mainBoard->Rules->Born, rulesString, &i);
    DrawString(rulesString, HELP_TEXT_X + 5, 140);

    DrawString("Cell Size:", HELP_TEXT_X, 149);
    sprintf(rulesString, "%d", mainBoard->CellHeight);
    DrawString(rulesString, HELP_TEXT_X + 5, 158);

    free(rulesString);
}

void DrawPlayPauseIcon(bool running) {
    #define COLUMN 287
    #define ROW 70
    DrawRectFill(COLUMN, ROW, 7, 11, 255);
    if (!running) {
        SetColorIndex(4);
        VerticalLine(COLUMN, ROW, 11);
        VerticalLine(COLUMN+1, ROW+1, 9);
        VerticalLine(COLUMN+2, ROW+2, 7);
        VerticalLine(COLUMN+3, ROW+3, 5);
        VerticalLine(COLUMN+4, ROW+4, 3);
        VerticalLine(COLUMN+5, ROW+5, 1);
    }
    else {
        DrawRectFill(COLUMN, ROW, 3, 9, 128);
        DrawRectFill(COLUMN+4, ROW, 3, 9, 128);
    }
}