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

#include <fileioc.h>
#include <graphc.h>

#include "key_helper.h"
#include "board.h"
#include "draw.h"
#include "menu.h"
#include "rule.h"
#include "main.h"
#include "topo_sprites.h"

uint8_t numRules;
Board* mainBoard;
Board* sampleBoard;
Rule* rulesList;

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

    mainBoard = NULL;
    LoadSettings();

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
    numRules = InitRules(&rulesList);
    SetRule(mainBoard, rulesList);
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

            DrawRectFill(237, 72, 80, 2, 0);

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
                Settings();
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
                DrawCell(mainBoard, old_x, old_y);
            }
        }
    }
    
    //DeleteBoard(sampleBoard);
    //SaveSettings();
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
    //ti_SetArchiveStatus(false, file);
    //file = ti_Open(appVarName, "w");

    //FreeCells(mainBoard);
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

    mainBoard = malloc(sizeof(Board));

    ti_Seek(23, SEEK_SET, file);
    if (!(ti_Read(mainBoard, sizeof(Board), 1, file) &&
        ti_Read(mainBoard->Rule, sizeof(Rule), 1, file))) {
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

void Settings() {
    Menu* menu = CreateMenu(6, "Settings");

    menu->Items[0].Name = "Colors...";
    menu->Items[0].Function = ColorSettings;
    
    menu->Items[1].Name = "Rules...";
    menu->Items[1].Function = RuleSettings;

    menu->Items[2].Name = "Topologies...";
    menu->Items[2].Function = TopologySettings;

    menu->Items[3].Name = "Cell Size...";
    menu->Items[3].Function = CellSizeSettings;

    menu->Items[4].Name = "Random Percentage...";
    menu->Items[4].Function = RandomPercentageSettings;
    
    menu->Items[5].Name = "Back";
    menu->Items[5].Function = FUNCTION_BACK;

    menu->BackKey = Key_Del;
    DisplayMenu(menu);
    DeleteMenu(menu);
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

void CellSizeSettings(MenuEventArgs* menuEventArgs) {
    uint8_t i;
    uint8_t previous;

    Menu* menu = CreateMenu(16, "Cell Size");
    menu->SelectionType = Single;

    menu->Items[0].Name = "1";
    menu->Items[1].Name = "2";
    menu->Items[2].Name = "3";
    menu->Items[3].Name = "4";
    menu->Items[4].Name = "5";
    menu->Items[5].Name = "6";
    menu->Items[6].Name = "7";
    menu->Items[7].Name = "8";
    menu->Items[8].Name = "9";
    menu->Items[9].Name = "10";
    menu->Items[10].Name = "11";
    menu->Items[11].Name = "12";
    menu->Items[12].Name = "13";
    menu->Items[13].Name = "14";
    menu->Items[14].Name = "15";
    menu->Items[15].Name = "Back";
    menu->Items[15].Function = FUNCTION_BACK;

    menu->Items[mainBoard->CellHeight - 1].Selected = true;
    previous = mainBoard->CellHeight - 1;
    
    menu->BackKey = Key_Del;
    DisplayMenu(menu);

    for (i = 0; i < menu->NumItems - 1; i++) {
        if (menu->Items[i].Selected && i != previous) {
            int newSize = 224 / (i + 1);
            if (newSize > 60) { newSize = 60; }

            ResizeBoard(mainBoard, newSize, newSize);
            
            mainBoard->CellHeight = i + 1;
            mainBoard->CellWidth = i + 1;
            break;
        }
    }

    DeleteMenu(menu);
}

void RandomPercentageSettings(MenuEventArgs* menuEventArgs) {

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

        Key_ScanKeys(125);

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

    for (i = 0; i < NUM_WRAPPING_MODES; i++) {
        menu->Items[i].Name = WrappingModeNames[i];
    }
    menu->Items[7].Name = "Back";
    menu->Items[7].Function = FUNCTION_BACK;

    menu->Items[mainBoard->WrappingMode].Selected = true;

    for (i = 0; i < NUM_WRAPPING_MODES; i++) {
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