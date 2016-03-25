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

#include "color_settings.h"

#include "board.h"
#include "draw.h"
#include "menu.h"

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

        Key_ScanKeys(150);

        old_i = i;
        old_j = j;

        if (Key_IsDown(Key_Up)) { j = j == 0 ? 7 : j - 1; } else if (Key_IsDown(Key_Down)) { j = j == 7 ? 0 : j + 1; } else if (Key_IsDown(Key_Left)) { i = i == 0 ? 31 : i - 1; } else if (Key_IsDown(Key_Right)) { i = i == 31 ? 0 : i + 1; } else if (Key_JustPressed(Key_2nd) || Key_JustPressed(Key_Enter)) {
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
        } else if (Key_JustPressed(Key_Del)) {
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