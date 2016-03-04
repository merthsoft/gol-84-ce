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
#define BOARD_HEIGHT	30
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

void SetTextColor(uint8_t foreground, uint8_t background);

void DrawBoard(bool);
void DrawGrid();
void SetupBoard();
void ClearBoard();
void Step();
uint8_t WrapToBoard(Board board, int c, int r, uint8_t layer, WrappingMode wrapRule);
void InitRules();

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

/* Put all your code here */
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
    
    //Debug_PrintLine("Entering main loop.");
	while (!Key_IsDown(Key_Mode)) {
		if (running) {
			Step();
			DrawBoard(false);
            
			kb_Scan();
			if (Key_IsDown(Key_Enter)) { running = false; }
		} else {
			gc_SetColorIndex(board[x][y][boardNumber] ? selAliveColor : selDeadColor);
			gc_NoClipRectangle(x*CELL_WIDTH + 1, y * CELL_HEIGHT + 1, CELL_WIDTH - 1, CELL_HEIGHT - 1);

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
			}

			if (Key_IsDown(Key_2nd)) {
				if (!toggled) {
					board[x][y][boardNumber] = !board[x][y][boardNumber];
					toggled = true;
				}
			} else {
				toggled = false;
			}

			if (old_x != x || old_y != y || toggled) {
				//Debug_PrintLine("Key pressed, redrawing cursor.");
				gc_SetColorIndex(board[old_x][old_y][boardNumber] ? aliveColor : deadColor);
				gc_NoClipRectangle(old_x*CELL_WIDTH + 1, old_y * CELL_HEIGHT + 1, CELL_WIDTH - 1, CELL_HEIGHT - 1);
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

uint8_t WrapToBoard(Board board, int c, int r, uint8_t layer, WrappingMode wrapRule){
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
				gc_SetColorIndex(board[i][j][boardNumber] ? aliveColor : deadColor);
				gc_NoClipRectangle(i*CELL_WIDTH + 1, j * CELL_HEIGHT + 1, CELL_WIDTH - 1, CELL_HEIGHT - 1);
			}
		}
	}
    //Debug_PrintLine("Board drawn.");

}

void DrawGrid() {
	uint8_t i;
	gc_SetColorIndex(gridColor);

	for (i = 1; i < BOARD_WIDTH; i++) {
		gc_NoClipVertLine(i*CELL_WIDTH, CELL_HEIGHT, (BOARD_HEIGHT - 2) * CELL_HEIGHT);
	}

	for (i = 1; i < BOARD_HEIGHT; i++) {
		gc_NoClipHorizLine(CELL_HEIGHT, i * CELL_HEIGHT, (BOARD_WIDTH - 2) * CELL_WIDTH);
	}
}