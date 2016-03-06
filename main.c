/* Keep these headers */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <ti84pce.h>

/* Standard headers - it's recommended to leave them included */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <debug.h>

#include <graphc.h>
#include <keypadc.h>

#include "key_helper.h"
#include "board.h"
#include "draw.h"

int Settings();

void main(void) {
	uint8_t x = 1;
	uint8_t y = 1;
	uint8_t old_x = x;
	uint8_t old_y = y;
	uint8_t i;

	bool toggled = false;
	bool running = false;
	
	//Debug_PrintLine("Initializing board.");
    ClearBoard();
	SetupBoard();
    InitRules();
	
    //Debug_PrintLine("Initial drawing.");
	gc_InitGraph();
	gc_FillScrn(255);
	DrawGrid();
	DrawBoard(true);
    
    kb_Scan();
    
	gc_PrintStringXY("2nd-Toggle", 235, 8);
	gc_PrintStringXY("Mode-Quit", 235, 17);
	gc_PrintStringXY("Clear-Clear", 235, 26);
	gc_PrintStringXY("Vars-Rand", 235, 35);
	gc_PrintStringXY("Enter-Run", 235, 44);
	gc_PrintStringXY("+-Step", 235, 53);
	gc_PrintStringXY("Del-Setup", 235, 62);

    //Debug_PrintLine("Entering main loop.");
	while (!Key_IsDown(Key_Mode)) {
		if (running) {
			Step();
			DrawBoard(false);
            
			kb_Scan();
			if (Key_IsDown(Key_Enter)) { running = false; }
		} else {
			DrawRectFill(x*CELL_WIDTH + 1, y * CELL_HEIGHT + 1, CELL_WIDTH - 1, CELL_HEIGHT - 1, board[x][y][boardNumber] ? selAliveColor : selDeadColor);

			for (i = 0; i < 100; i++) { kb_Scan(); }

			old_x = x;
			old_y = y;

			if (Key_IsDown(Key_Up)) { y = y == 1 ? BOARD_HEIGHT - 2 : y - 1; }
			else if (Key_IsDown(Key_Down)) { y = y == BOARD_HEIGHT - 2 ? 1 : y + 1; }
			else if (Key_IsDown(Key_Left)) { x = x == 1 ? BOARD_WIDTH - 2 : x - 1; }
			else if (Key_IsDown(Key_Right)) { x = x == BOARD_WIDTH - 2 ? 1 : x + 1; }
			else if (Key_IsDown(Key_Enter)) { running = true; }
			else if (Key_IsDown(Key_Clear)) { 
				ClearBoard();
				DrawBoard(true);
			} else if (Key_IsDown(Key_Alpha)) {
				Settings();
				DrawBoard(true);
			}

			if (Key_IsDown(Key_2nd)) {
				if (!toggled) {
					board[x][y][boardNumber] = !board[x][y][boardNumber];
					toggled = true;
				}
			} else {
				toggled = false;
			}

			if (old_x != x || old_y != y || running) {
				//Debug_PrintLine("Key pressed, redrawing cursor.");
				DrawRectFill(old_x*CELL_WIDTH + 1, old_y * CELL_HEIGHT + 1, CELL_WIDTH - 1, CELL_HEIGHT - 1, board[old_x][old_y][boardNumber] ? aliveColor : deadColor);
			}
		}

        kb_Scan();
	}
	   
	gc_CloseGraph();
    kb_Reset();
}

int Settings() {
	uint8_t i;
	uint8_t y = 1;
	uint8_t old_y = 1;
	unsigned char out[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	uint8_t newValues[5];
	newValues[0] = gridColor;
	newValues[1] = deadColor;
	newValues[2] = aliveColor;
	newValues[3] = selDeadColor; 
	newValues[4] = selAliveColor;

	gc_FillScrn(255);
	while (1) {
		gc_PrintStringXY("Settings:", 0, 0);
		//IntToHex(newValues[0], out);
		gc_PrintStringXY("Grid:", 9, 9);
		DrawRectFill(270, 48, 24, 26, newValues[0]);
		
		//IntToHex(newValues[1], out);
		gc_PrintStringXY("Dead:", 9, 18);
		DrawRectFill(270, 72, 24, 26, newValues[1]);
		
		//IntToHex(newValues[2], out);
		gc_PrintStringXY("Alive:", 9, 27);
		DrawRectFill(270, 96, 24, 26, newValues[2]);
		
		//IntToHex(newValues[3], out);
		gc_PrintStringXY("SelDead:", 9, 36);
		DrawRectFill(270, 120, 24, 26, newValues[3]);
		
		//IntToHex(newValues[4], out);
		gc_PrintStringXY("SelAlive:", 9, 45);
		DrawRectFill(270, 144, 24, 26, newValues[4]);

		gc_PrintStringXY(">", 0, 9 * y);
		
		for (i = 0; i < 100; i++) { kb_Scan(); }

		old_y = y;
		
		if (Key_IsDown(Key_Up)) { y = y == 1 ? 5 : y - 1;	}
		else if (Key_IsDown(Key_Down)) { y = y == 5 ? 1 : y + 1; }
		else if (Key_IsDown(Key_Left)) { newValues[y - 1] -= 0x0001; }
		else if (Key_IsDown(Key_Right)) { newValues[y - 1] += 0x0001; }
		else if (Key_IsDown(Key_Add)) { newValues[y - 1] -= 0x0010; }
		else if (Key_IsDown(Key_Sub)) { newValues[y - 1] += 0x0010; }
		else if (Key_IsDown(Key_Mul)) { newValues[y - 1] -= 0x0100; }
		else if (Key_IsDown(Key_Div)) { newValues[y - 1] += 0x0100; }
		
		if (Key_IsDown(Key_Mode)) { return 0; }

		if (old_y != y) {
			DrawRectFill(0, 9 * old_y, 8, 8, 255);
		}
/*
		case KEY_CTRL_DEL:
			newValues[y - 1] -= 0x1000;
			break;
		case KEY_CTRL_AC:
			newValues[y - 1] += 0x1000;
			break;
		case KEY_CTRL_F5:
			return KEY_PRGM_F5;
			break;
		case KEY_CTRL_F2:
			newValues[0] = 0x0000;
			newValues[1] = 0x0000;
			newValues[2] = 0xF010;
			newValues[3] = 0xFD00;
			newValues[4] = 0x7890;
			break;
		case KEY_CTRL_F6:
			gridColor = newValues[0];
			deadColor = newValues[1];
			aliveColor = newValues[2];
			selDeadColor = newValues[3];
			selAliveColor = newValues[4];
			return KEY_PRGM_F6;
			break;
		case KEY_CTRL_F3:
			Rules();
			break;
		case KEY_CTRL_F4:
			About();
			break;
		}*/
	}
}