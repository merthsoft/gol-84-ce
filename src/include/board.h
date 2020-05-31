#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

#include "rules.h"

typedef enum {
    Plane   = 0,
    Ring    = 1,
    Mobius  = 2,
    Torus   = 3,
    Sphere  = 4,
    Klein   = 5,
    Proj    = 6
} WrappingMode;

typedef struct {
    uint8_t BoardWidth;
    uint8_t BoardHeight;
    uint8_t BoardNumber;
    WrappingMode WrappingMode;
    uint8_t GridColor;
    uint8_t DeadColor;
    uint8_t AliveColor;
    uint8_t CellWidth;
    uint8_t CellHeight;
    uint8_t CursorAliveColor;
    uint8_t CursorDeadColor;
    uint8_t RandomChance;
    uint8_t CursorX;
    uint8_t CursorY;
    uint8_t OffsetX;
    uint8_t OffsetY;
    Rules* Rules;
    uint8_t** Cells[2];
} Board;

Board* CreateBoard(uint8_t boardWidth, uint8_t boardHeight);
void DeleteBoard(Board* board);
void ResizeBoard(Board* b, uint8_t boardWidth, uint8_t boardHeight);
void DrawBoard(Board* board, bool redraw);
void DrawGrid(Board* board);
void RandomBoard(Board* board);
void ClearBoard(Board* board);
void FillBoard(Board* board);
void Step(Board* board);
uint8_t WrapToBoard(Board* board, uint8_t c, uint8_t r);
void DrawCell(Board* board);
void DrawCursor(Board* board);
void SetRules(Board* b, Rules* rules);
void FreeCells(Board* b);

#endif BOARD_H