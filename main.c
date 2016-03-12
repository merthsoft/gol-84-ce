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

#include <graphc.h>
#include <keypadc.h>

#include "key_helper.h"
#include "board.h"
#include "draw.h"
#include "menu.h"
#include "rules.h"

#define BOARD_WIDTH		28
#define BOARD_HEIGHT	BOARD_WIDTH

#define CELL_WIDTH		8
#define CELL_HEIGHT		8

void Settings();
void ColorSettings(Menu* parentMenu, int parentIndex);
void TopologySettings(Menu* parentMenu, int parentIndex);
void DrawSampleBoard(Menu* parentMenu, int parentIndex);
void ColorPicker(Menu* parentMenu, int parentIndex);

Board* mainBoard;
Board* sampleBoard;

void main(void) {
	uint8_t x = 1;
	uint8_t y = 1;
	uint8_t old_x = x;
	uint8_t old_y = y;
	uint8_t i;

	bool redraw = true;
	bool toggled = false;
	bool running = false;
	
	gc_InitGraph();
	gc_FillScrn(0);

	mainBoard = CreateBoard(BOARD_WIDTH, BOARD_HEIGHT);
	mainBoard->AliveColor = 18;
	mainBoard->DeadColor = 255;
	mainBoard->GridColor = 0;
	mainBoard->WrappingMode = Plane;
	mainBoard->CellHeight = CELL_HEIGHT;
	mainBoard->CellWidth = CELL_WIDTH;
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
    mainBoard->Rule = &rules[0];
    
    kb_Scan();

	while (!Key_IsDown(Key_Del)) {
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
            
			kb_Scan();
			if (Key_IsDown(Key_Enter)) { running = false; }
		} else {
			DrawCursor(mainBoard, x, y, 0, 0);

			for (i = 0; i < 100; i++) { kb_Scan(); }

			old_x = x;
			old_y = y;

			if (Key_IsDown(Key_Up)) { y = y == 1 ? mainBoard->BoardHeight : y - 1; }
			else if (Key_IsDown(Key_Down)) { y = y == mainBoard->BoardHeight ? 1 : y + 1; }
			else if (Key_IsDown(Key_Left)) { x = x == 1 ? mainBoard->BoardWidth : x - 1; }
			else if (Key_IsDown(Key_Right)) { x = x == mainBoard->BoardWidth ? 1 : x + 1; }
			else if (Key_IsDown(Key_Enter)) { running = true; }
			else if (Key_IsDown(Key_Clear)) { 
				ClearBoard(mainBoard);
				DrawBoard(mainBoard, true, 0, 0);
			} else if (Key_IsDown(Key_Mode)) {
				Settings();
				redraw = true;
			} else if (Key_IsDown(Key_Add)) {
				Step(mainBoard);
				DrawBoard(mainBoard, false, 0, 0);
			}

			if (Key_IsDown(Key_2nd)) {
				if (!toggled) {
					mainBoard->Cells[mainBoard->BoardNumber][x][y] = !mainBoard->Cells[mainBoard->BoardNumber][x][y];
					toggled = true;
				}
			} else {
				toggled = false;
			}

			if (old_x != x || old_y != y || running) {
				DrawCell(mainBoard, old_x, old_y);
			}
		}

        kb_Scan();
	}
	
	kb_Reset();
	gc_CloseGraph();
	pgrm_CleanUp();
}

void Settings() {
    Menu* menu = CreateMenu(4, "Settings");

	menu->Items[0].Name = "Colors";
    menu->Items[0].Function = ColorSettings;
	
    menu->Items[1].Name = "Rules";

    menu->Items[2].Name = "Topologies";
    menu->Items[2].Function = TopologySettings;
    
    menu->Items[3].Name = "Back";
    menu->Items[3].Function = FUNCTION_BACK;

    DisplayMenu(menu);
    DeleteMenu(menu);
}

void ColorSettings(Menu* parentMenu, int parentIndex) {
    uint8_t i;

    Menu* menu = CreateMenu(7, "Colors");
    menu->ExtraFunction = DrawSampleBoard;

	menu->Items[0].Name = "Grid";
	menu->Items[1].Name = "Dead";
	menu->Items[2].Name = "Alive";
	menu->Items[3].Name = "Cursor (Dead)";
	menu->Items[4].Name = "Cursor (Alive)";
	menu->Items[5].Name = "Save";
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

    DisplayMenu(menu);
    DeleteMenu(menu);
}

void DrawSampleBoard(Menu* parentMenu, int parentIndex) {
	gc_PrintStringXY("Sample:", 150, 0);
	DrawGrid(sampleBoard, 150, 9);
	DrawBoard(sampleBoard, true, 150, 9);
    DrawCursor(sampleBoard, 1, 1, 150, 9);
    DrawCursor(sampleBoard, 2, 1, 150, 9);
}

void ColorPicker(Menu* parentMenu, int parentIndex) {
    uint8_t i, j, c = 0;
    
    gc_SetColorIndex(0);
    gc_ClipHorizLine(0, 85, 320);
    for (j = 0; j < 8; j++) {
        for (i = 0; i < 32; i++) {
            DrawRectFill(10 * i + 1, 10 * j + 100, 8, 8, c++);
        }
    }
    SetTextColor(0, 255);
    gc_PrintStringXY("Select color for: ", 1, 87);
    gc_PrintStringXY(parentMenu->Items[parentIndex].Name, 120, 87);

    switch (parentIndex) {
        case 0:
            c = mainBoard->GridColor;
        case 1:
            c = mainBoard->DeadColor;
        case 2:
            c = mainBoard->AliveColor;
        case 3:
            c = mainBoard->CursorDeadColor;
        case 4:
            c = mainBoard->CursorAliveColor;
    }
    
    i = c % 32;
    j = c / 32;

    while (!Key_IsDown(Key_Del)) {
        kb_Scan();

        gc_SetColorIndex(0);
        gc_NoClipRectangleOutline(10 * i + 1, 10 * j + 100, 8, 8);
    }
}

void TopologySettings(Menu* parentMenu, int parentIndex) {
    Menu* menu = CreateMenu(8, "Topologies:");
    menu->SelectionType = Single;

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

    DisplayMenu(menu);
    DeleteMenu(menu);
}