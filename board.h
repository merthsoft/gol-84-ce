#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "rules.h"
#include "draw.h"

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

uint8_t gridColor = 0;
uint8_t deadColor = 255;
uint8_t aliveColor = 18;
uint8_t selDeadColor = 224;
uint8_t selAliveColor = 15;

WrappingMode wrapMode = Plane;

int randMod = 4;

uint8_t boardNumber = 0;
typedef uint8_t Board[BOARD_WIDTH][BOARD_HEIGHT][2];
Board board;

void DrawBoard(bool);
void DrawGrid();
void SetupBoard();
void ClearBoard();
void Step();
uint8_t WrapToBoard(Board, uint8_t, uint8_t, uint8_t, WrappingMode);

#endif BOARD_H