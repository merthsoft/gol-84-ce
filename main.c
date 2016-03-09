/* Keep these headers */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>
//#include <stdio.h>
//#include <debug.h>

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

void Settings();
void ColorSettings();

uint8_t newValues[5];
void DrawSampleBoard();
Board* mainBoard;

//void Debug_Print(char * format, ...) {
//    va_list argptr;
//    va_start(argptr, format);
//    dbg_printf(dbgout, format, argptr);
//    va_end(argptr);
//}
//
//void Debug_PrintLine(char * format, ...) {
//	va_list argptr;
//	va_start(argptr, format);
//	dbg_printf(dbgout, format, argptr);
//	va_end(argptr);
//	dbg_printf(dbgout, "\n");
//}

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

	//Debug_PrintLine("Creating board.");
	mainBoard = CreateBoard(BOARD_WIDTH, BOARD_HEIGHT);
	//Debug_PrintLine("Board created.");
	mainBoard->AliveColor = 18;
	mainBoard->DeadColor = 255;
	mainBoard->GridColor = 0;
	mainBoard->WrappingMode = Plane;
	mainBoard->CellHeight = CELL_HEIGHT;
	mainBoard->CellWidth = CELL_WIDTH;
	mainBoard->CursorDeadColor = 224;
	mainBoard->CursorAliveColor = 15;

	//Debug_PrintLine("Initializing board.");
    ClearBoard(mainBoard);
	SetupBoard(mainBoard);
    InitRules(mainBoard);
    
    kb_Scan();
    //Debug_PrintLine("Entering main loop.");
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
			DrawGrid(mainBoard);
			DrawBoard(mainBoard, true);

			redraw = false;
		}

		if (running) {
			Step(mainBoard);
			DrawBoard(mainBoard, false);
            
			kb_Scan();
			if (Key_IsDown(Key_Enter)) { running = false; }
		} else {
			DrawCursor(mainBoard, x, y);

			for (i = 0; i < 100; i++) { kb_Scan(); }

			old_x = x;
			old_y = y;

			if (Key_IsDown(Key_Up)) { y = y == 1 ? BOARD_HEIGHT - 2 : y - 1; }
			else if (Key_IsDown(Key_Down)) { y = y == BOARD_HEIGHT - 2 ? 1 : y + 1; }
			else if (Key_IsDown(Key_Left)) { x = x == 1 ? BOARD_WIDTH - 2 : x - 1; }
			else if (Key_IsDown(Key_Right)) { x = x == BOARD_WIDTH - 2 ? 1 : x + 1; }
			else if (Key_IsDown(Key_Enter)) { running = true; }
			else if (Key_IsDown(Key_Clear)) { 
				ClearBoard(mainBoard);
				DrawBoard(mainBoard, true);
			} else if (Key_IsDown(Key_Mode)) {
				Settings();
				redraw = true;
			} else if (Key_IsDown(Key_Add)) {
				Step(mainBoard);
				DrawBoard(mainBoard, false);
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
				//Debug_PrintLine("Key pressed, redrawing cursor.");
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
	menuItem items[4];
	items[0].name = "Colors";
	items[0].function = ColorSettings;
	items[1].name = "Rules";
	items[2].name = "Topologies";
	items[3].name = "Back";
	items[3].function = BACK_FUNCTION;

	Menu("Settings:", items, 4, NULL);
}

void ColorSettings() {
	menuItem items[6];
	items[0].name = "Grid";
	items[1].name = "Dead";
	items[2].name = "Alive";
	items[3].name = "Selected (Dead)";
	items[4].name = "Selected (Alive)";
	items[5].name = "Save";
	items[6].name = "Back";
	items[6].function = BACK_FUNCTION;

	newValues[0] = mainBoard->GridColor;
	newValues[1] = mainBoard->DeadColor;
	newValues[2] = mainBoard->AliveColor;
	newValues[3] = mainBoard->CursorDeadColor;
	newValues[4] = mainBoard->CursorAliveColor;

	Menu("Colors:", items, 6, DrawSampleBoard);
}

void DrawSampleBoard() {

}