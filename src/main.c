#include <keypadc.h>
#include <time.h>
#include <stdio.h>
#include <string.h> 

#include "gfx/gfx.h"
#include "include/board.h"
#include "include/draw.h"
#include "include/menu.h"
#include "include/rules.h"
#include "include/settings.h"
#include "include/const.h"
#include "include/stamp_picker.h"
#include "include/stamp.h"

void DrawPlayPauseIcon(bool running, color foreground, color background);
void DrawHelpText(Board* mainBoard, bool running);

const char appVarName[] = "[GoL";


int main(void) {
    uint8_t x, old_x;
    uint8_t y, old_y;
    Board mainBoard;
    memset(&mainBoard, 0, sizeof(Board));
    RulesList* rulesList;
    Stamp* selectedStamp = NULL;
    
    bool redraw = true;
    bool running = false;
    bool quit = false;
    
    rulesList = GetRules();

    LoadSettings(&mainBoard, appVarName, rulesList);
    
    if (!mainBoard.IsInitialized) {
        InitializeBoard(&mainBoard, 0, 0);
        SquareCells(&mainBoard, 10, MAX_BOARD_DRAW_SIZE, MAX_BOARD_SIZE);

        mainBoard.AliveColor = 18;
        mainBoard.DeadColor = 255;
        mainBoard.GridColor = 0;
        mainBoard.CursorDeadColor = 224;
        mainBoard.CursorAliveColor = 15;
        
        mainBoard.WrappingMode = Torus;
        mainBoard.RandomChance = 50;

        SetRules(&mainBoard, rulesList->List);
    }
    
    GraphicsBegin();
    ClearScreen();
    gfx_SetTransparentColor(0xff);
    
    while(kb_AnyKey());
    bool keyPressed = false;
    clock_t clockOffset = 0;

    while (!quit) {
        if (redraw) {
            DrawHelpText(&mainBoard, running);
            redraw = false;
        }
        
        if (running) {
            Step(&mainBoard);
            
            kb_Scan();
            if (keyPressed == false && kb_AnyKey()) {
                if (kb_IsDown(kb_KeyEnter)) 
                {
                    while(kb_AnyKey());
                    keyPressed = true;
                    running = false;
                    DrawPlayPauseIcon(running, mainBoard.AliveColor, mainBoard.DeadColor);
                }
                else if (kb_IsDown(kb_KeyDel)) 
                    quit = true;
            } else {
                if (!kb_AnyKey()) {
                    keyPressed = false;
                }
            }
        } else {
            DrawCursor(&mainBoard, selectedStamp, false);
                       
            x = old_x = mainBoard.CursorX;
            y = old_y = mainBoard.CursorY;

             // If no key is pressed, reset timer and variable for keeping track of if a key is held down.
            if (clockOffset == 0 || (!kb_AnyKey() && keyPressed)) {
                keyPressed = false;
                clockOffset = clock();
            }

            // If no key is being held, allow an input
            // If a key is being held, introduce a small delay between inputs. In this example the delay is
            // CLOCKS_PER_SEC / 32, but it can be changed to what feels most natural.
            if (kb_AnyKey() && (!keyPressed || clock() - clockOffset > CLOCKS_PER_SEC / 8)) {
                clockOffset = clock();

                if (kb_IsDown(kb_KeyUp)) 
                    y = y == 1 ? mainBoard.BoardHeight : y - 1;
                else if (kb_IsDown(kb_KeyDown)) 
                    y = y == mainBoard.BoardHeight ? 1 : y + 1;
                else if (kb_IsDown(kb_KeyLeft))
                    x = x == 1 ? mainBoard.BoardWidth : x - 1;
                else if (kb_IsDown(kb_KeyRight)) 
                    x = x == mainBoard.BoardWidth ? 1 : x + 1;
                else if (kb_IsDown(kb_KeyDel)) 
                    quit = selectedStamp == NULL ? true : (selectedStamp = NULL) == NULL;
                else if (kb_IsDown(kb_KeyAdd))
                    Step(&mainBoard);
                else if (kb_IsDown(kb_KeyMode)) {
                    Settings(&mainBoard, rulesList);
                    redraw = true;
                } else if (kb_IsDown(kb_KeyVars)) {
                    RandomBoard(&mainBoard);
                    redraw = true;
                } else if (kb_IsDown(kb_KeyEnter)) {
                    DrawPlayPauseIcon(true, mainBoard.AliveColor, mainBoard.DeadColor);
                    running = true;
                } else if (kb_IsDown(kb_KeyAlpha)){ 
                    // selectedStamp = ChooseStamp(&mainBoard);
                    // if (selectedStamp == NULL)
                    //     dbg_sprintf(dbgout, "Stamp NULL\n");
                    // else
                    //     dbg_sprintf(dbgout, "Stamp %.20s\n", selectedStamp->Name);
                    // redraw = true;
                } else if (kb_IsDown(kb_KeyClear)) {
                    ClearBoard(&mainBoard);
                    DrawBoard(&mainBoard, true);
                } else if (kb_IsDown(kb_KeyPower)) {
                    FillBoard(&mainBoard);
                    DrawBoard(&mainBoard, true);
                } else if (kb_IsDown(kb_Key2nd)) {
                    if (selectedStamp == NULL) {
                        ToggleCell(&mainBoard);
                    } else {
                        PlaceStamp(&mainBoard, selectedStamp);
                    }
                }
                // If this is the first instance of an input after a release (the key has not already been held
                // down for some time), wait for a longer delay to ensure the user wants to continue holding down
                // the key and creating more inputs. In this example the delay is CLOCKS_PER_SEC / 4, but it can
                // be changed to what feels most natural.
                if (!keyPressed) {
                    while (clock() - clockOffset < CLOCKS_PER_SEC / 4 && kb_AnyKey()) {
                        kb_Scan();
                    }
                }

                // Now we know that the user is holding down a key (If not, we'll reset keyPressed back to false
                // at the beginning of the loop.
                keyPressed = true;

                if (old_x != x || old_y != y || running) {
                    DrawCursor(&mainBoard, selectedStamp, true);
                }
            }
            
            mainBoard.CursorX = x;
            mainBoard.CursorY = y;
        }
    }
    
    SaveSettings(&mainBoard, appVarName);
    DeleteBoard(&mainBoard);

    GraphicsEnd();

    return 0;
}

void DrawHelpText(Board* board, bool running) {
    uint8_t i = 0;
    char* rulesString = malloc(sizeof(char) * 11);
    memset(rulesString, 0, 11);

    FillScreen(board->DeadColor);
    
    if (board->GridColor == board->DeadColor) {
        SetTextColor(board->CursorDeadColor, board->DeadColor);
    } else {
        SetTextColor(board->GridColor, board->DeadColor);
    }
    DrawString("2nd-Toggle", HELP_TEXT_X, 8);
    DrawString("Del-Quit", HELP_TEXT_X, 17);
    DrawString("Clear-Clear", HELP_TEXT_X, 26);
    DrawString("^-Fill", HELP_TEXT_X, 35);
    DrawString("Vars-Rand", HELP_TEXT_X, 44);
    DrawString("+-Step", HELP_TEXT_X, 53);
    DrawString("Mode-Setup", HELP_TEXT_X, 62);
    DrawString("Enter-", HELP_TEXT_X, 71);
    DrawString("v1.4", 292, 228);
    DrawGrid(board);
    DrawBoard(board, true);
    DrawPlayPauseIcon(running, board->AliveColor, board->DeadColor);

    DrawRectFill(239, 81, 80, 2, 0);

    DrawString("Topology:", HELP_TEXT_X, 86);
    DrawString(WrappingModeNames[board->WrappingMode], HELP_TEXT_X + 3, 95);
    DrawString("Rules:", HELP_TEXT_X, 104);

    DrawString("Stay alive:", HELP_TEXT_X + 5, 113);
    memset(rulesString, 0, 11);
    i = 0;
    NumToRuleString(board->Rules.Live, rulesString, &i);
    DrawString(rulesString, HELP_TEXT_X + 5, 122);

    DrawString("Born:", HELP_TEXT_X + 5, 131);
    memset(rulesString, 0, 11);
    i = 0;
    NumToRuleString(board->Rules.Born, rulesString, &i);
    DrawString(rulesString, HELP_TEXT_X + 5, 140);

    DrawString("Cell Size:", HELP_TEXT_X, 149);
    sprintf(rulesString, "%d", board->CellHeight);
    DrawString(rulesString, HELP_TEXT_X + 5, 158);

    free(rulesString);
}

void DrawPlayPauseIcon(bool running, color foreground, color background) {
    #define COLUMN 287
    #define ROW 70
    DrawRectFill(COLUMN, ROW, 7, 11, background);
    if (!running) {
        SetColorIndex(foreground);
        VerticalLine(COLUMN, ROW, 11);
        VerticalLine(COLUMN+1, ROW+1, 9);
        VerticalLine(COLUMN+2, ROW+2, 7);
        VerticalLine(COLUMN+3, ROW+3, 5);
        VerticalLine(COLUMN+4, ROW+4, 3);
        VerticalLine(COLUMN+5, ROW+5, 1);
    }
    else {
        DrawRectFill(COLUMN, ROW, 3, 9, foreground);
        DrawRectFill(COLUMN+4, ROW, 3, 9, foreground);
    }
}