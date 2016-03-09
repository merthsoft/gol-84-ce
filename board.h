#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "rules.h"
#include "draw.h"

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

int randMod = 4;

typedef struct {
	uint8_t BoardWidth;
	uint8_t BoardHeight;
	uint8_t** Cells[2];
	uint8_t BoardNumber;
	WrappingMode WrappingMode;
	uint8_t GridColor;
	uint8_t DeadColor;
	uint8_t AliveColor;
	uint8_t CellWidth;
	uint8_t CellHeight;
	uint8_t CursorAliveColor;
	uint8_t CursorDeadColor;
} Board;

Board* CreateBoard(uint8_t, uint8_t);
void DrawBoard(Board*, bool, uint8_t, uint8_t);
void DrawGrid(Board*, uint8_t, uint8_t);
void SetupBoard(Board*);
void ClearBoard(Board*);
void Step(Board*);
uint8_t WrapToBoard(Board* board, uint8_t c, uint8_t r);
void DrawCell(Board*, uint8_t, uint8_t);
void DrawCursor(Board*, uint8_t, uint8_t);

#endif BOARD_H