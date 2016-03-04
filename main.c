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

/* Other available headers */
// stdarg.h, setjmp.h, ctype.h, float.h, iso646.h, limits.h, errno.h, assert.h, debug.h

#define SCREEN_WIDTH	320
#define SCREEN_HEIGHT	240

#define LCD_WIDTH_PX	320

#define CELL_WIDTH		8
#define CELL_HEIGHT		8

#define BOARD_WIDTH		30 
#define CYCLE_WIDTH (BOARD_WIDTH - 2)
#define BOARD_HEIGHT	BOARD_WIDTH
#define CYCLE_HEIGHT (BOARD_HEIGHT - 2)
#define MOD_CYCLE(n, c) (((n) + (c)) % (c))

typedef enum {
	Plane = 0,
	Ring = 1,
	Mobius = 2,
	Torus = 3,
	Sphere = 4,
	Klein = 5,
	Proj = 6
} WrappingMode;

typedef struct {
	short live;
	short born;
	char* name;
} rule;

uint8_t gridColor = 0;
uint8_t deadColor = 255;
uint8_t aliveColor = 18;
uint8_t selDeadColor = 224;
uint8_t selAliveColor = 15;

uint8_t numRules = 6;
rule* rules;

WrappingMode wrapMode = Plane;

short live = 0x0C;
short born = 0x08;

uint8_t boardNumber = 0;
typedef uint8_t Board[BOARD_WIDTH][BOARD_HEIGHT][2];
Board board;

int randMod = 4;

void SetTextColor(uint8_t, uint8_t);

void DrawBoard(bool);
void DrawGrid();
void SetupBoard();
void ClearBoard();
void Step();
uint8_t WrapToBoard(Board, uint8_t, uint8_t, uint8_t, WrappingMode);
void InitRules();
void DrawRectFill(int, int, int, int, uint8_t);
int Settings();

//void Debug_Print(char * format, ...) {
//	va_list argptr;
//	va_start(argptr, format);
//	dbg_printf(dbgout, format, argptr);
//	va_end(argptr);
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

void SetTextColor(uint8_t foreground, uint8_t background) {
	gc_SetTextColor(foreground | background << 8);
}

void DrawRectFill(int x, int y, int w, int h, uint8_t color_index) {
	gc_SetColorIndex(color_index);
	gc_NoClipRectangle(x, y, w, h);
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

void InitRules() {
	rules = (rule*)malloc(numRules*sizeof(rule));
	// Life: 23/3
	rules[0].live = 0x0C;
	rules[0].born = 0x08;
	rules[0].name = "Life:      23/3";
	// HighLife: 23/36
	rules[1].live = 0x0C;
	rules[1].born = 0x48;
	rules[1].name = "HighLife:  23/36";
	// Replicator: 1357/1357
	rules[2].live = 0xAA;
	rules[2].born = 0xAA;
	rules[2].name = "Replicate: 1357/1357";
	// Maze: 12345/3
	rules[3].live = 0x3E;
	rules[3].born = 0x08;
	rules[3].name = "Maze:      12345/3";
	// 34 Life: 34/34
	rules[4].live = 0x18;
	rules[4].born = 0x18;
	rules[4].name = "34 Life:   34/34";
	// 2x2: 125/36
	rules[5].live = 0x26;
	rules[5].born = 0x09;
	rules[5].name = "2x2:       125/36";
}

void Step() {
	uint8_t numN; // The number of neighbors
	uint8_t i;
	uint8_t j;

	for (i = 1; i < BOARD_WIDTH - 1; i++) {
		for (j = 1; j < BOARD_HEIGHT - 1; j++) {
			numN = 0;
			// Count the 8 cells around it
			numN += WrapToBoard(board,i - 1, j - 1, boardNumber, wrapMode)
			+ WrapToBoard(board,i,j - 1,boardNumber,wrapMode) + WrapToBoard(board,i + 1,j - 1,boardNumber,wrapMode) +
				WrapToBoard(board,i - 1,j,boardNumber,wrapMode) + WrapToBoard(board,i + 1,j,boardNumber,wrapMode) +
				WrapToBoard(board,i - 1,j + 1,boardNumber,wrapMode) + WrapToBoard(board,i,j + 1,boardNumber,wrapMode) + WrapToBoard(board,i + 1,j + 1,boardNumber,wrapMode);
			// Rules:
			if (!(live & (1 << numN))) {
				board[i][j][!boardNumber] = 0;
			}
			else {
				board[i][j][!boardNumber] = board[i][j][boardNumber];
			}

			if ((born & (1 << numN)) && board[i][j][boardNumber] == 0) {
				board[i][j][!boardNumber] = 1;
			}
		}
	}
	boardNumber = !boardNumber; // Use the new board as the current board
}

uint8_t WrapToBoard(Board board, uint8_t c, uint8_t r, uint8_t layer, WrappingMode wrapRule){
	int lC, lR;
	bool lE = c == 0, rE = c > CYCLE_WIDTH,
	tE = r == 0, bE = r > CYCLE_HEIGHT,
	ovC = lE || rE,
	ovR = tE || bE;
	switch (wrapRule) {
		case Plane:
			lC = c; lR = r;
			break;
		case Ring:
			lC = ovC ? (MOD_CYCLE(c - 1, CYCLE_WIDTH) + 1) : c;
			lR = r;
			break;
		case Mobius:
			lC = ovC ? (MOD_CYCLE(c - 1, CYCLE_WIDTH) + 1) : c;
			lR = ovC ? (BOARD_HEIGHT - r) : r ;
			break;
		case Torus:
			lC = ovC ? (MOD_CYCLE(c - 1, CYCLE_WIDTH) + 1) : c;
			lR = ovR ? (MOD_CYCLE(r - 1, CYCLE_HEIGHT) + 1) : r;
			break;
		case Sphere:
		{
			/* This only makes sense if the board is square. Otherwise, all hell will break loose */
			uint8_t mask = ((uint8_t)lE << 3)|((uint8_t)rE << 2)|((uint8_t)tE << 1)|((uint8_t)bE);
			switch (mask) {
				case 0xA:
				case 0x5:
					return 0;
				case 0x9:
					lC = CYCLE_WIDTH;
					lR = 1;
					break;
				case 0x8:
					lC = r;
					lR = 1;
					break;
				case 0x6:
					lC = 1;
					lR = CYCLE_HEIGHT;
					break;
				case 0x4:
					lC = r;
					lR = CYCLE_HEIGHT;
					break;
				case 0x2:
					lC = 1;
					lR = c;
					break;
				case 0x1:
					lC = CYCLE_WIDTH;
					lR = c;
					break;
				default:
					lC = c;
					lR = r;
			}
			break;
		}
		case Klein:
			lC = ovC ? (MOD_CYCLE(c - 1, CYCLE_WIDTH) + 1) : c;
			lR = ovR ? (MOD_CYCLE(r - 1, CYCLE_HEIGHT) + 1) : r;
			lR = ovC ? (BOARD_HEIGHT - lR) : lR;
			break;
		case Proj:
			lC = ovC ? (MOD_CYCLE(c - 1, CYCLE_WIDTH) + 1) : c;
			lC = ovR ? (BOARD_WIDTH - lC) : lC;
			lR = ovR ? (MOD_CYCLE(r - 1, CYCLE_HEIGHT) + 1) : r;
			lR = ovC ? (BOARD_HEIGHT - lR) : lR;
			break;
	}
	return board[lC][lR][layer];
}

void SetupBoard() {
	uint8_t i, j;
    srand(*(unsigned long*)0xF30044);

	for (i = 1; i < BOARD_WIDTH - 1; i++) {
		for (j = 1; j < BOARD_HEIGHT - 1; j++) {
			board[i][j][boardNumber] = !(rand() % randMod);
			board[i][j][!boardNumber] = 0;
		}
	}
}

void ClearBoard() {
	uint8_t i, j;
	for (i = 0; i < BOARD_WIDTH + 1; i++) {
		for (j = 0; j < BOARD_HEIGHT + 1; j++) {
			board[i][j][boardNumber] = 0;
			board[i][j][!boardNumber] = 0;
		}
	}
}

void DrawBoard(bool redraw) {
	uint8_t i, j;
	
	//Debug_PrintLine("Drawing board.");
	for (i = 1; i < BOARD_WIDTH - 1; i++) {
		for (j = 1; j < BOARD_HEIGHT - 1; j++) {
			if (board[i][j][boardNumber] != board[i][j][!boardNumber] || redraw) {
				DrawRectFill(i*CELL_WIDTH + 1, j * CELL_HEIGHT + 1, CELL_WIDTH - 1, CELL_HEIGHT - 1, board[i][j][boardNumber] ? aliveColor : deadColor);
			}
		}
	}
    //Debug_PrintLine("Board drawn.");

}

void DrawGrid() {
	uint8_t i;
	gc_SetColorIndex(gridColor);

	for (i = 1; i < BOARD_WIDTH; i++) {
		gc_NoClipVertLine(i*CELL_WIDTH, CELL_HEIGHT, (BOARD_HEIGHT - 2) * CELL_HEIGHT + 1);
	}

	for (i = 1; i < BOARD_HEIGHT; i++) {
		gc_NoClipHorizLine(CELL_HEIGHT, i * CELL_HEIGHT, (BOARD_WIDTH - 2) * CELL_WIDTH + 1);
	}
}