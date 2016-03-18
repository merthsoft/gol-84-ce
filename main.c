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

#include <graphc.h>

#include "key_helper.h"
#include "board.h"
#include "draw.h"
#include "menu.h"
#include "rule.h"
#include "main.h"
#include "topo_sprites.h"

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

    mainBoard = CreateBoard(boardWidth, boardHeight);
    mainBoard->AliveColor = 18;
    mainBoard->DeadColor = 255;
    mainBoard->GridColor = 0;
    mainBoard->WrappingMode = Torus;
    mainBoard->CellHeight = cellWidth;
    mainBoard->CellWidth = cellHeight;
    mainBoard->CursorDeadColor = 224;
    mainBoard->CursorAliveColor = 15;
    mainBoard->RandomMod = 4;

    sampleBoard = CreateBoard(5, 5);
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
    
    ClearBoard(mainBoard);
    SetupBoard(mainBoard);
    InitRules();
    Key_Init();

    while (!quit) {
        if (redraw) {
            gc_FillScrn(255);
            gc_PrintStringXY("2nd-Toggle", 235, 8);
            gc_PrintStringXY("Del-Quit", 235, 17);
            gc_PrintStringXY("Clear-Clear", 235, 26);
            gc_PrintStringXY("Vars-Rand", 235, 35);
            gc_PrintStringXY("Enter-Run", 235, 44);
            gc_PrintStringXY("+-Step", 235, 53);
            gc_PrintStringXY("Mode-Setup", 235, 62);
            DrawGrid(mainBoard, 0, 0);
            DrawBoard(mainBoard, true, 0, 0);     

            redraw = false;
        }
        
        if (running) {
            Step(mainBoard);
            DrawBoard(mainBoard, false, 0, 0);
            
            Key_ScanKeys(false);
            if (Key_JustPressed(Key_Enter)) { running = false; }
            else if (Key_JustPressed(Key_Del)) { quit = true; }
        } else {
            DrawCursor(mainBoard, x, y, 0, 0);

            Key_ScanKeys(true);

            old_x = x;
            old_y = y;

            if (Key_IsDown(Key_Up)) { y = y == 1 ? mainBoard->BoardHeight : y - 1; }
            else if (Key_IsDown(Key_Down)) { y = y == mainBoard->BoardHeight ? 1 : y + 1; }
            else if (Key_IsDown(Key_Left)) { x = x == 1 ? mainBoard->BoardWidth : x - 1; }
            else if (Key_IsDown(Key_Right)) { x = x == mainBoard->BoardWidth ? 1 : x + 1; }
            else if (Key_JustPressed(Key_Enter)) { running = true; }
            else if (Key_IsDown(Key_Clear)) { 
                ClearBoard(mainBoard);
                DrawBoard(mainBoard, true, 0, 0);
            } else if (Key_IsDown(Key_Mode)) {
                Settings();
                redraw = true;
            } else if (Key_IsDown(Key_Add)) {
                Step(mainBoard);
                DrawBoard(mainBoard, false, 0, 0);
            } else if (Key_IsDown(Key_Del)) { quit = true; } 
            else if (Key_IsDown(Key_Vars)) {
                SetupBoard(mainBoard);
                redraw = true;
            }

            if (Key_JustPressed(Key_2nd)) {
                mainBoard->Cells[mainBoard->BoardNumber][x][y] = !mainBoard->Cells[mainBoard->BoardNumber][x][y];
                toggled = true;
            }

            if (old_x != x || old_y != y || running) {
                DrawCell(mainBoard, old_x, old_y);
            }
        }
    }
    
    Key_Reset();
    gc_CloseGraph();
    pgrm_CleanUp();
}

void InitRules() {
    numRules = 12;
    rulesList = malloc(numRules * sizeof(Rule));

    rulesList[0].Name = "Life (B3/S3)";
    rulesList[0].Live = 0x0C;
    rulesList[0].Born = 0x08;

    rulesList[1].Name = "HighLife (B36/S23)";
    rulesList[1].Live = 0x0C;
    rulesList[1].Born = 0x48;

    rulesList[2].Name = "Replicator (B1357/S1357)";
    rulesList[2].Live = 0xAA;
    rulesList[2].Born = 0xAA;

    rulesList[3].Name = "Maze (B3/S12345)";
    rulesList[3].Live = 0x3E;
    rulesList[3].Born = 0x08;

    rulesList[4].Name = "34 Life (B34/S34)";
    rulesList[4].Live = 0x18;
    rulesList[4].Born = 0x18;

    rulesList[5].Name = "2x2 (B36/S125)";
    rulesList[5].Live = 0x26;
    rulesList[5].Born = 0x09;

    rulesList[6].Name = "Life without Death";
    rulesList[6].Live = 0xFF;
    rulesList[6].Born = 0x08;

    rulesList[7].Name = "Seeds (B2/S)";
    rulesList[7].Live = 0x00;
    rulesList[7].Born = 0x04;

    rulesList[8].Name = "Diamoeba (B35678/S5678)";
    rulesList[8].Live = 0x1E0;
    rulesList[8].Born = 0x1E8;

    rulesList[9].Name = "Day & Night (B3678/S34678)";
    rulesList[9].Live = 0x1D8;
    rulesList[9].Born = 0x1C8;

    rulesList[10].Name = "Morley (B368/S245)";
    rulesList[10].Live = 0x34;
    rulesList[10].Born = 0x148;

    rulesList[11].Name = "Anneal (B4678/S35678)";
    rulesList[11].Live = 0x1E8;
    rulesList[11].Born = 0x1D0;

    SetRule(mainBoard, &rulesList[0]);
}

void Settings() {
    Menu* menu = CreateMenu(4, "Settings");

    menu->Items[0].Name = "Colors...";
    menu->Items[0].Function = ColorSettings;
    
    menu->Items[1].Name = "Rules...";
    menu->Items[1].Function = RuleSettings;

    menu->Items[2].Name = "Topologies...";
    menu->Items[2].Function = TopologySettings;
    
    menu->Items[3].Name = "Back";
    menu->Items[3].Function = FUNCTION_BACK;

    menu->BackKey = Key_Del;
    DisplayMenu(menu);
    DeleteMenu(menu);
}

void RuleSettings(MenuEventArgs* menuEventArgs) {
    int i;
    bool foundRule = false;

    Menu* menu = CreateMenu(numRules + 2, "Rules");
    menu->SelectionType = Single;

    for (i = 0; i < numRules; i++) {
        menu->Items[i].Name = rulesList[i].Name;
        menu->Items[i].Function = SetRuleMenuEvent;
        if (rulesList[i].Born == mainBoard->Rule->Born && rulesList[i].Live == mainBoard->Rule->Live) {
            menu->Items[i].Selected = true;
            foundRule = true;
        }
    }

    menu->Items[numRules].Name = "Custom...";

    menu->Items[numRules + 1].Name = "Back";
    menu->Items[numRules + 1].Function = FUNCTION_BACK;

    menu->BackKey = Key_Del;
    DisplayMenu(menu);
    DeleteMenu(menu);
}

void SetRuleMenuEvent(MenuEventArgs* menuEventArgs) {
    SetRule(mainBoard, &rulesList[menuEventArgs->Index]);
}

void ColorSettings(MenuEventArgs* menuEventArgs) {
    uint8_t i;

    Menu* menu = CreateMenu(7, "Colors");
    menu->ExtraFunction = DrawSampleBoard;

    menu->Items[0].Name = "Grid...";
    menu->Items[1].Name = "Dead...";
    menu->Items[2].Name = "Alive...";
    menu->Items[3].Name = "Cursor (Dead)...";
    menu->Items[4].Name = "Cursor (Alive)...";
    menu->Items[5].Name = "Save";
    menu->Items[5].Function = SaveColors;
    menu->Items[6].Name = "Back";
    menu->Items[6].Function = FUNCTION_BACK;

    for (i = 0; i < 5; i++) {
        menu->Items[i].Function = ColorPicker;
    }

    sampleBoard->GridColor = mainBoard->GridColor;
    sampleBoard->DeadColor = mainBoard->DeadColor;
    sampleBoard->AliveColor = mainBoard->AliveColor;
    sampleBoard->CursorDeadColor = mainBoard->CursorDeadColor;
    sampleBoard->CursorAliveColor = mainBoard->CursorAliveColor;

    menu->BackKey = Key_Del;
    DisplayMenu(menu);
    DeleteMenu(menu);
}

void DrawSampleBoard(MenuEventArgs* menuEventArgs) {
    if (menuEventArgs->FrameNumber == 0) {
        gc_PrintStringXY("Sample:", 150, 0);
        DrawGrid(sampleBoard, 150, 9);
        DrawBoard(sampleBoard, true, 150, 9);
        DrawCursor(sampleBoard, 1, 1, 150, 9);
        DrawCursor(sampleBoard, 2, 1, 150, 9);
    }
}

void ColorPicker(MenuEventArgs* menuEventArgs) {
    uint8_t a = 0;
    uint8_t i, j, c = 0;
    uint8_t old_i, old_j;
    
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

        Key_ScanKeys(true);

        old_i = i;
        old_j = j;

        if (Key_IsDown(Key_Up)) { j = j == 0 ? 7 : j - 1; }
        else if (Key_IsDown(Key_Down)) { j = j == 7 ? 0 : j + 1; }
        else if (Key_IsDown(Key_Left)) { i = i == 0 ? 31 : i - 1; }
        else if (Key_IsDown(Key_Right)) { i = i == 31 ? 0 : i + 1; }
        else if (Key_JustPressed(Key_2nd) || Key_JustPressed(Key_Enter)) {
            MenuEventArgs m;
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

            m.FrameNumber = 0;
            m.Index = 0;
            m.Menu = NULL;
            DrawSampleBoard(&m);
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
    mainBoard->GridColor = sampleBoard->GridColor;
    mainBoard->DeadColor = sampleBoard->DeadColor;
    mainBoard->AliveColor = sampleBoard->AliveColor;
    mainBoard->CursorDeadColor = sampleBoard->CursorDeadColor;
    mainBoard->CursorAliveColor = sampleBoard->CursorAliveColor;
    menuEventArgs->Back = true;
}

void TopologySettings(MenuEventArgs* menuEventArgs) {
    int i;

    Menu* menu = CreateMenu(8, "Topologies");
    menu->SelectionType = Single;
    menu->ExtraFunction = DrawTopoSprite;

    menu->Items[0].Name = "Plane";
    menu->Items[1].Name = "Ring";
    menu->Items[2].Name = "Mobius";
    menu->Items[3].Name = "Torus";
    menu->Items[4].Name = "Sphere";
    menu->Items[5].Name = "Klein";
    menu->Items[6].Name = "Projection";
    menu->Items[7].Name = "Back";
    menu->Items[7].Function = FUNCTION_BACK;

    menu->Items[mainBoard->WrappingMode].Selected = true;

    for (i = 0; i < 6; i++) {
        menu->Items[i].Function = DrawTopoSprite;
    }

    menu->BackKey = Key_Del;
    DisplayMenu(menu);

    for (i = 0; i < 6; i++) {
        if (menu->Items[i].Selected) {
            mainBoard->WrappingMode = i;
            break;
        }
    }

    DeleteMenu(menu);
}

void DrawTopoSprite(MenuEventArgs* menuEventArgs) {
    uint8_t* sprite = NULL;

    switch (menuEventArgs->Index) {
        case 0:
            sprite = topo_plane;
            break;
        case 1:
            sprite = topo_ring;
            break;
        case 2:
            sprite = topo_mobius;
            break;
        case 3:
            sprite = topo_torus;
            break;
        case 4:
            sprite = topo_sphere;
            break;
        case 5:
            sprite = topo_klein;
            break;
        case 6:
            sprite = topo_proj;
            break;
        default:
            sprite = topo_plane;
            break;
    }

    gc_NoClipDrawScaledSprite(sprite, 150, 9, 16, 16, 9, 9);
}