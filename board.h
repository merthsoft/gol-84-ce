#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "rule.h"

#define MOD_CYCLE(n, c) (((n) + (c)) % (c))

typedef enum {
    Plane   = 0,
    Ring    = 1,
    Mobius  = 2,
    Torus   = 3,
    Sphere  = 4,
    Klein   = 5,
    Proj    = 6
} WrappingMode;

#define NUM_WRAPPING_MODES (Proj+1)

char* WrappingModeNames[NUM_WRAPPING_MODES] = {
    "Plane",
    "Ring",
    "Mobius",
    "Torus",
    "Sphere",
    "Klein",
    "Proj"
};

typedef struct Board {
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
    uint8_t RandomMod;
    Rule* Rule;
} Board;

Board* CreateBoard(uint8_t boardWidth, uint8_t boardHeight);
void DeleteBoard(Board* board);
void ResizeBoard(Board* b, uint8_t boardWidth, uint8_t boardHeight);
void DrawBoard(Board* board, bool redraw, uint8_t offsetx, uint8_t offsety);
void DrawGrid(Board* board, uint8_t offsetx, uint8_t offsety);
void RandomBoard(Board* board);
void ClearBoard(Board* board);
void Step(Board* board);
uint8_t WrapToBoard(Board* board, uint8_t c, uint8_t r);
void DrawCell(Board* board, uint8_t x, uint8_t y, uint8_t offsetx, uint8_t offsety);
void DrawCursor(Board* board, uint8_t x, uint8_t y, uint8_t offsetx, uint8_t offsety);
void SetRule(Board* b, Rule* rule);
void FreeCells(Board* b);

#endif BOARD_H