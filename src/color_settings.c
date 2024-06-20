#include <keypadc.h>
#include <time.h>

#include "include/color_settings.h"
#include "include/board.h"
#include "include/draw.h"
#include "include/const.h"

void DrawSampleBoard(MenuEventArgs* menuEventArgs);
void ColorPicker(MenuEventArgs* menuEventArgs);
void SaveColors(MenuEventArgs* menuEventArgs);

void ColorSettings(MenuEventArgs* menuEventArgs) {
    uint8_t i;
    Board* mainBoard = menuEventArgs->Menu->Tag;
    Menu* menu;

    Board sampleBoard;
    InitializeBoard(&sampleBoard, 5, 5);
    sampleBoard.OffsetX = 150;
    sampleBoard.OffsetY = 9;
    sampleBoard.CellHeight = mainBoard->CellHeight;
    sampleBoard.CellWidth = mainBoard->CellWidth;
    
    sampleBoard.CursorX = 2;
    sampleBoard.CursorY = 2;
    RulesList* rulesList = GetRules();
    PlaceStamp(&sampleBoard, rulesList->List[0].Stamps[0]);
    sampleBoard.Cells[0][15] = 1;

    sampleBoard.GridColor = mainBoard->GridColor;
    sampleBoard.DeadColor = mainBoard->DeadColor;
    sampleBoard.AliveColor = mainBoard->AliveColor;
    sampleBoard.CursorDeadColor = mainBoard->CursorDeadColor;
    sampleBoard.CursorAliveColor = mainBoard->CursorAliveColor;

    menu = CreateMenu(7, "Colors");
    menu->FillColor = mainBoard->DeadColor;
    menu->ExtraFunction = DrawSampleBoard;

    menu->Items[0].Name = "Grid...";
    menu->Items[1].Name = "Dead...";
    menu->Items[2].Name = "Alive...";
    menu->Items[3].Name = "Cursor (Dead)...";
    menu->Items[4].Name = "Cursor (Alive)...";
    menu->Items[5].Name = "Save";
    menu->Items[5].Function = SaveColors;
    menu->Items[5].Tag = &sampleBoard;
    menu->Items[6].Name = (char*)BackString;
    menu->Items[6].Function = FUNCTION_BACK;

    for (i = 0; i < 5; i++) {
        menu->Items[i].Function = ColorPicker;
        menu->Items[i].Tag = &sampleBoard;
    }

    menu->Tag = mainBoard;

    DisplayMenu(menu);
    DeleteMenu(menu);
    DeleteBoard(&sampleBoard);
}

void __drawSampleBoard(Board* board) {
    DrawString("Sample:", 150, 0);
    DrawGrid(board);
    DrawBoard(board, true);
    board->CursorX = 1;
    board->CursorY = 1;
    DrawCursor(board, NULL, false);
    board->CursorX = 2;
    board->CursorY = 1;
    DrawCursor(board, NULL, false);
}

void DrawSampleBoard(MenuEventArgs* menuEventArgs) {
    if (menuEventArgs->FrameNumber == 0) {
        __drawSampleBoard(menuEventArgs->Item->Tag);
    }
}

void ColorPicker(MenuEventArgs* menuEventArgs) {
    uint8_t i, j, c = 0;
    uint8_t old_i, old_j;

    Board* mainBoard = menuEventArgs->Menu->Tag;
    Board* sampleBoard = menuEventArgs->Menu->Items[menuEventArgs->Index].Tag;

    SetColorIndex(0);
    HorizontalLine(0, 85, 320);
    for (j = 0; j < 8; j++) {
        for (i = 0; i < 32; i++) {
            DrawRectFill(10 * i + 1, 10 * j + 100, 8, 8, c++);
        }
    }
    SetTextColor(0, 255);
    DrawString("Select color for: ", 1, 87);
    DrawString(menuEventArgs->Menu->Items[menuEventArgs->Index].Name, 120, 87);

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
    
    while(kb_AnyKey());
    bool keyPressed = false;
    clock_t clockOffset = 0;
    while (true) {
        DrawRect(10 * i, 10 * j + 99, 10, 10, 0);

        old_i = i;
        old_j = j;
        
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
            if (kb_IsDown(kb_KeyUp)) { j = j == 0 ? 7 : j - 1; } 
            else if (kb_IsDown(kb_KeyDown)) { j = j == 7 ? 0 : j + 1; } 
            else if (kb_IsDown(kb_KeyLeft)) { i = i == 0 ? 31 : i - 1; } 
            else if (kb_IsDown(kb_KeyRight)) { i = i == 31 ? 0 : i + 1; } 
            else if (kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) {
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
            } else if (kb_IsDown(kb_KeyDel)) {
                menuEventArgs->FrameNumber = -1; // Force a re-draw of the sample board
                return;
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

            if (i != old_i || j != old_j) {
                DrawRect(10 * old_i, 10 * old_j + 99, 10, 10, 255);
            }
        }
    }

    while(kb_AnyKey());
}

void SaveColors(MenuEventArgs* menuEventArgs) {
    Board* mainBoard = menuEventArgs->Menu->Tag;
    Board* sampleBoard = menuEventArgs->Menu->Items[menuEventArgs->Index].Tag;

    mainBoard->GridColor = sampleBoard->GridColor;
    mainBoard->DeadColor = sampleBoard->DeadColor;
    mainBoard->AliveColor = sampleBoard->AliveColor;
    mainBoard->CursorDeadColor = sampleBoard->CursorDeadColor;
    mainBoard->CursorAliveColor = sampleBoard->CursorAliveColor;
    menuEventArgs->Back = true;
}