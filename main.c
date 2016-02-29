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
#define BOARD_HEIGHT	30

typedef struct {
	short live;
	short born;
	char* name;
} rule;

int gridColor = 0;
int deadColor = 255;
int aliveColor = 18;
int selDeadColor = 224;
int selAliveColor = 15;

int numRules = 6;
rule* rules;

short live = 0x0C;
short born = 0x08;

int boardNumber = 0;
uint8_t board[BOARD_WIDTH + 2][BOARD_HEIGHT + 2][2];

int randMod = 4;

void SetTextColor(uint8_t foreground, uint8_t background);

void DrawBoard();
void SetupBoard();
void ClearBoard();
void Step();
void InitRules();


/* Put all your code here */
void main(void) {
	int x = 1;
	int y = 1;
	int old_x = x;
	int old_y = y;
	int i;

	bool toggled = false;
	bool running = false;
	
    ClearBoard();
	SetupBoard();
    InitRules();
	
	gc_InitGraph();
	gc_FillScrn(255);
	DrawBoard();

    kb_Scan();

	while (!Key_IsDown(Key_Mode)) {
		if (running) {
			Step();
			DrawBoard();
            
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
				DrawBoard();
			}

			if (Key_IsDown(Key_2nd)) {
				if (!toggled) {
					board[x][y][boardNumber] = !board[x][y][boardNumber];
					toggled = true;
				}
			} else {
				toggled = false;
			}

			if (old_x != x || old_y != y) { DrawBoard(); }
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
	int numN; // The number of neighbors
	int i;
	int j;

	for (i = 1; i < BOARD_WIDTH - 1; i++) {
		for (j = 1; j < BOARD_HEIGHT - 1; j++) {
			numN = 0;
			// Count the 8 cells around it
			numN += board[i - 1][j - 1][boardNumber] + board[i][j - 1][boardNumber] + board[i + 1][j - 1][boardNumber] +
				board[i - 1][j][boardNumber] + board[i + 1][j][boardNumber] +
				board[i - 1][j + 1][boardNumber] + board[i][j + 1][boardNumber] + board[i + 1][j + 1][boardNumber];
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

void SetupBoard() {
	int i, j;
    srand(*(unsigned long*)0xF30044);

	for (i = 1; i < BOARD_WIDTH - 1; i++) {
		for (j = 1; j < BOARD_HEIGHT - 1; j++) {
			board[i][j][boardNumber] = !(rand() % randMod);
			board[i][j][!boardNumber] = 0;
		}
	}
}

void ClearBoard() {
	int i, j;
	for (i = 0; i < BOARD_WIDTH + 1; i++) {
		for (j = 0; j < BOARD_HEIGHT + 1; j++) {
			board[i][j][boardNumber] = 0;
			board[i][j][!boardNumber] = 0;
		}
	}
}

void DrawBoard() {
	int i, j;
	for (i = 1; i < BOARD_WIDTH - 1; i++) {
		for (j = 1; j < BOARD_HEIGHT - 1; j++) {
			gc_SetColorIndex(board[i][j][boardNumber] ? aliveColor : deadColor);
			gc_NoClipRectangle(i*CELL_WIDTH, j * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT);

			gc_SetColorIndex(gridColor);
			gc_NoClipHorizLine(i*CELL_WIDTH, j * CELL_HEIGHT, CELL_WIDTH);
			gc_NoClipVertLine(i*CELL_WIDTH, j * CELL_HEIGHT, CELL_HEIGHT);
		}
	}

	gc_NoClipHorizLine(CELL_HEIGHT, (BOARD_HEIGHT - 1) * CELL_HEIGHT, (BOARD_WIDTH - 2) * CELL_WIDTH);
	gc_NoClipVertLine((BOARD_WIDTH - 1) * CELL_WIDTH, CELL_WIDTH, (BOARD_HEIGHT - 2) * CELL_HEIGHT);
}