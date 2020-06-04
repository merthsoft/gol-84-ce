#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>

#include "rules.h"
#include "draw.h"

#define MAX_BOARD_DRAW_SIZE 224
#define MAX_BOARD_SIZE      80
#define HELP_TEXT_X         240

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
    color GridColor;
    color DeadColor;
    color AliveColor;
    color CursorDeadColor;
    color CursorAliveColor;
    uint8_t CellWidth;
    uint8_t CellHeight;
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
void SquareCells(Board* board, uint8_t cellSize, uint8_t drawSize, uint8_t maxSize);
void DrawBoard(Board* board, bool redraw);
void PlaceStamp(Board* board, Stamp* stamp);
void DrawGrid(Board* board);
void RandomBoard(Board* board);
void ClearBoard(Board* board);
void FillBoard(Board* board);
void Step(Board* board);
uint8_t WrapToBoard(Board* board, uint8_t c, uint8_t r);
void DrawCell(Board* board, bool clear);
void ToggleCell(Board* board);
void DrawCursor(Board* board, Stamp* stamp, bool clear);
void SetRules(Board* b, Rules* rules);
void FreeCells(Board* b);

#endif BOARD_H