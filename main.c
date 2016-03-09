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

#define BOARD_WIDTH		28
#define BOARD_HEIGHT	BOARD_WIDTH

#define CELL_WIDTH		8
#define CELL_HEIGHT		8

void Settings();
void ColorSettings();

void DrawSampleBoard();
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

	sampleBoard = CreateBoard(5, 5);
	sampleBoard->CellHeight = mainBoard->CellHeight;
	sampleBoard->CellWidth = mainBoard->CellWidth;

	sampleBoard->Cells[0][2][1] = 1;
	sampleBoard->Cells[0][3][2] = 1;
	sampleBoard->Cells[0][1][3] = 1;
	sampleBoard->Cells[0][2][3] = 1;
	sampleBoard->Cells[0][3][3] = 1;

    ClearBoard(mainBoard);
	SetupBoard(mainBoard);
    InitRules(mainBoard);
    
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
			DrawCursor(mainBoard, x, y);

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
	MenuItem items[4];
	items[0].Name = "Colors";
	items[0].Function = ColorSettings;
	items[1].Name = "Rules";
	items[2].Name = "Topologies";
	items[3].Name = "Back";
	items[3].Function = BACK_FUNCTION;

	Menu("Settings:", items, 4, NULL);
}

void ColorSettings() {
	MenuItem items[7];
	items[0].Name = "Grid";
	items[1].Name = "Dead";
	items[2].Name = "Alive";
	items[3].Name = "Selected (Dead)";
	items[4].Name = "Selected (Alive)";
	items[5].Name = "Save";
	items[6].Name = "Back";
	items[6].Function = BACK_FUNCTION;

	sampleBoard->GridColor = mainBoard->GridColor;
	sampleBoard->DeadColor = mainBoard->DeadColor;
	sampleBoard->AliveColor = mainBoard->AliveColor;
	sampleBoard->CursorDeadColor = mainBoard->CursorDeadColor;
	sampleBoard->CursorAliveColor = mainBoard->CursorAliveColor;

	Menu("Colors:", items, 7, DrawSampleBoard);
}

void DrawSampleBoard() {
	gc_PrintStringXY("Sample:", 150, 0);
	DrawGrid(sampleBoard, 150, 9);
	DrawBoard(sampleBoard, true, 150, 9);
}