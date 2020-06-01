#include <stdint.h>
#include <stdlib.h>
#include <tice.h>
#include <string.h>

#include "include/board.h"
#include "include/draw.h"
#include "include/key_helper.h"

#define MOD_CYCLE(n, c) (((n) + (c)) % (c))

void Step(Board* board) {
    uint8_t numN; // The number of neighbors
    uint8_t i, j;
    bool cellStatus;
    uint8_t boardNum = board->BoardNumber;
    Rules* currentRule = board->Rules;
    uint8_t cellWidth = board->CellWidth;
    uint8_t cellHeight = board->CellHeight;

    for (i = 1; i <= board->BoardWidth; i++) {
        for (j = 1; j <= board->BoardHeight; j++) {
            numN = 0;
            // Count the 8 cells around it
            numN += WrapToBoard(board, i - 1, j - 1)
                    + WrapToBoard(board, i, j - 1) 
                    + WrapToBoard(board, i + 1, j - 1) 
                    + WrapToBoard(board, i - 1, j) 
                    + WrapToBoard(board, i + 1, j) 
                    + WrapToBoard(board, i - 1, j + 1) 
                    + WrapToBoard(board, i, j + 1) 
                    + WrapToBoard(board, i + 1, j + 1);
            
            cellStatus = board->Cells[boardNum][i][j];
            // Rules:
            if (!(currentRule->Live & (1 << numN))) {
                cellStatus = 0;
            }
            if ((currentRule->Born & (1 << numN)) && board->Cells[boardNum][i][j] == 0) {
                cellStatus = 1;
            }

            board->Cells[!boardNum][i][j] = cellStatus;

            if (board->Cells[boardNum][i][j] != cellStatus) {
                uint8_t color = cellStatus ? board->AliveColor : board->DeadColor;
                if (cellWidth > 2) {
                    DrawRectFill(i*cellWidth + 1, j * cellHeight + 1, cellWidth - 1, cellHeight - 1, color);
                } else {
                    DrawRectFill(i*cellWidth, j * cellHeight, cellWidth, cellHeight, color);
                }
            }
        }
    }
    board->BoardNumber = !boardNum; // Use the new board as the current board
}

uint8_t WrapToBoard(Board* board, uint8_t c, uint8_t r) {
    uint8_t CYCLE_WIDTH = board->BoardWidth;
    uint8_t CYCLE_HEIGHT = board->BoardHeight;

    uint8_t lC = 0;
    uint8_t lR = 0;

    bool lE = c == 0;
    bool tEE = r == 0;
    bool rE = c > CYCLE_WIDTH;
    bool bE = r > CYCLE_HEIGHT;
    bool ovC = lE || rE;
    bool ovR = tEE || bE;

    if (!ovC && !ovR || board->WrappingMode == Plane) { return board->Cells[board->BoardNumber][c][r]; }

    switch (board->WrappingMode) {
        case Plane:
            lC = c; lR = r;
            break;
        case Ring:
            lC = ovC ? (MOD_CYCLE(c - 1, CYCLE_WIDTH) + 1) : c;
            lR = r;
            break;
        case Mobius:
            lC = ovC ? (MOD_CYCLE(c - 1, CYCLE_WIDTH) + 1) : c;
            lR = ovC ? (CYCLE_HEIGHT - r + 2) : r;
            break;
        case Torus:
            lC = ovC ? (MOD_CYCLE(c - 1, CYCLE_WIDTH) + 1) : c;
            lR = ovR ? (MOD_CYCLE(r - 1, CYCLE_HEIGHT) + 1) : r;
            break;
        case Sphere:
        {
            /* This only makes sense if the board is square. Otherwise, all hell will break loose */
            uint8_t mask = ((uint8_t)lE << 3) | ((uint8_t)rE << 2) | ((uint8_t)tEE << 1) | ((uint8_t)bE);
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
                    break;
            }
            break;
    }
    case Klein:
        lC = ovC ? (MOD_CYCLE(c - 1, CYCLE_WIDTH) + 1) : c;
        lR = ovR ? (MOD_CYCLE(r - 1, CYCLE_HEIGHT) + 1) : r;
        lR = ovC ? (CYCLE_HEIGHT - lR + 2) : lR;
        break;
    case Proj:
        lC = ovC ? (MOD_CYCLE(c - 1, CYCLE_WIDTH) + 1) : c;
        lC = ovR ? (CYCLE_WIDTH - lC + 2) : lC;
        lR = ovR ? (MOD_CYCLE(r - 1, CYCLE_HEIGHT) + 1) : r;
        lR = ovC ? (CYCLE_HEIGHT - lR + 2) : lR;
        break;
    }
    return board->Cells[board->BoardNumber][lC][lR];
}

void __colorCell(Board* board, uint8_t x, uint8_t y, uint8_t offsetx, uint8_t offsety, uint8_t color) {
    uint8_t cellWidth = board->CellWidth;
    uint8_t cellHeight = board->CellHeight;

    if (cellWidth > 2) {
        DrawRectFill(x*cellWidth + 1 + offsetx, y * cellHeight + 1 + offsety, cellWidth - 1, cellHeight - 1, color);
    } else {
        DrawRectFill(x*cellWidth + offsetx, y * cellHeight + offsety, cellWidth, cellHeight, color);
    }
}

void DrawCell(Board* board) {
    uint8_t offsetx = board->OffsetX;
    uint8_t offsety = board->OffsetY;
    uint8_t x = board->CursorX;
    uint8_t y = board->CursorY;
    __colorCell(board, x, y, offsetx, offsety, board->Cells[board->BoardNumber][x][y] ? board->AliveColor : board->DeadColor);
}

void DrawCursor(Board* board) {
    uint8_t offsetx = board->OffsetX;
    uint8_t offsety = board->OffsetY;
    uint8_t x = board->CursorX;
    uint8_t y = board->CursorY;
    __colorCell(board, x, y, offsetx, offsety, board->Cells[board->BoardNumber][x][y] ? board->CursorAliveColor : board->CursorDeadColor);
}

void ToggleCell(Board* board) {
    uint8_t x = board->CursorX;
    uint8_t y = board->CursorY;
    board->Cells[board->BoardNumber][x][y] = !board->Cells[board->BoardNumber][x][y];
}

void PlaceStamp(Board* board, Stamp* stamp) {
    uint8_t i, j;
    uint8_t offsetx = board->OffsetX;
    uint8_t offsety = board->OffsetY;
    uint8_t x = board->CursorX;
    uint8_t y = board->CursorY;
    uint8_t width = board->CellWidth;
    uint8_t height = board->CellHeight;

    for (i = 0; i < stamp->Width; i++)
        for (j = 0; j < stamp->Height; j++)
            board->Cells[board->BoardNumber][x + i][y + j] = stamp->Cells[i][j];
}

void DrawStamp(Board* board, Stamp* stamp) {
    uint8_t i, j;
    uint8_t offsetx = board->OffsetX;
    uint8_t offsety = board->OffsetY;
    uint8_t x = board->CursorX;
    uint8_t y = board->CursorY;
    uint8_t** cells = board->Cells[board->BoardNumber];
    uint8_t width = board->CellWidth;
    uint8_t height = board->CellHeight;

    for (i = 0; i < stamp->Width; i++) {
        for (j = 0; j < stamp->Height; j++) {
            uint8_t color = stamp->Cells[i][j]
                            ? cells[x + i][y + j]
                              ? board->CursorAliveColor
                              : board->CursorDeadColor
                            : board->DeadColor;
            __colorCell(board, x + i, y + j, offsetx, offsety, color);
        }
    }
}

void ClearStamp(Board* board, Stamp* stamp) {
    uint8_t i, j;
    uint8_t offsetx = board->OffsetX;
    uint8_t offsety = board->OffsetY;
    uint8_t x = board->CursorX;
    uint8_t y = board->CursorY;
    uint8_t width = board->CellWidth;
    uint8_t height = board->CellHeight;

    for (i = x; i < x + stamp->Width; i++)
        for (j = y; j < y + stamp->Height; j++)
            __colorCell(board, i, j, offsetx, offsety, board->Cells[board->BoardNumber][i][j] ? board->AliveColor : board->DeadColor);
}

void RandomBoard(Board* board) {
    uint8_t i, j;
    srand(rtc_Time());

    for (i = 1; i <= board->BoardWidth + 1; i++) {
        for (j = 1; j <= board->BoardHeight + 1; j++) {
            board->Cells[0][i][j] = (rand() % 100) < board->RandomChance;
            board->Cells[1][i][j] = !(board->Cells[0][i][j]);
        }
    }

    board->BoardNumber = 0;
}

void __setValue(Board* board, uint8_t val) {
    uint8_t i, j;
    for (i = 0; i < board->BoardWidth + 2; i++) {
        for (j = 0; j < board->BoardHeight + 2; j++) {
            board->Cells[0][i][j] = val;
            board->Cells[1][i][j] = val;
        }
    }

    board->BoardNumber = 0;
}

void ClearBoard(Board* board) {
    __setValue(board, 0);
}

void FillBoard(Board* board) {
    __setValue(board, 1);
}

void __allocCells(Board* b) {
    uint8_t i, j;
    size_t mallocSize;

    for (i = 0; i < 2; i++) {
        b->Cells[i] = malloc(sizeof(uint8_t*) * (b->BoardWidth + 2));
        for (j = 0; j < b->BoardWidth + 2; j++) {
            mallocSize = sizeof(uint8_t*) * (b->BoardHeight + 2);
            
            b->Cells[i][j] = malloc(mallocSize);
            memset(b->Cells[i][j], 0, mallocSize);
        }
    }
}

Board* CreateBoard(uint8_t boardWidth, uint8_t boardHeight) {
    int i;
    int j;
    Board* b = malloc(sizeof(Board));
    memset(b, 0, sizeof(Board));
    b->CursorX = 1;
    b->CursorY = 1;
    b->BoardWidth = boardWidth;
    b->BoardHeight = boardHeight;

    __allocCells(b);

    b->BoardNumber = 0;
    b->Rules = malloc(sizeof(Rules));
    b->Rules->Live = 0;
    b->Rules->Born = 0;
    b->Rules->Name = NULL;
    return b;
}

void DeleteBoard(Board* b) {
    FreeCells(b);

    if (b->Rules != NULL)
        free(b->Rules);
    free(b);
}

void ResizeBoard(Board* b, uint8_t boardWidth, uint8_t boardHeight) {
    FreeCells(b);

    b->BoardWidth = boardWidth;
    b->BoardHeight = boardHeight;

    b->CursorX = 1;
    b->CursorY = 1;

    __allocCells(b);
    ClearBoard(b);
}

void FreeCells(Board* b) {
    uint8_t i;
    uint8_t j;

    if (b->Cells[0] == NULL)
        return;
    
    for (i = 0; i < 2; i++) {
        for (j = 0; j < b->BoardWidth + 2; j++) {
            free(b->Cells[i][j]);
            b->Cells[i][j] = NULL;
        }
        free(b->Cells[i]);
        b->Cells[i] = NULL;
    }
}

void SetRules(Board* b, Rules* rules) {
    b->Rules->Live = rules->Live;
    b->Rules->Born = rules->Born;
    b->Rules->Name = rules->Name;
    b->Rules->NumStamps = rules->NumStamps;
    b->Rules->Stamps = rules->Stamps;
}

void DrawBoard(Board* board, bool redraw) {
    uint8_t i, j;
    uint8_t offsetx = board->OffsetX;
    uint8_t offsety = board->OffsetY;
    uint8_t boardNumber = board->BoardNumber;
    uint8_t cellWidth = board->CellWidth;
    uint8_t cellHeight = board->CellHeight;

    for (i = 1; i <= board->BoardWidth; i++)
        for (j = 1; j <= board->BoardHeight; j++)
            if (board->Cells[boardNumber][i][j] != board->Cells[!boardNumber][i][j] || redraw)
                __colorCell(board, i, j, offsetx, offsety, board->Cells[boardNumber][i][j] ? board->AliveColor : board->DeadColor);
}

void DrawGrid(Board* board) {
    uint8_t i;
    uint8_t offsetx = board->OffsetX;
    uint8_t offsety = board->OffsetY;
    uint8_t cellWidth = board->CellWidth;
    uint8_t cellHeight = board->CellHeight;

    if (board->CellHeight <= 2) { return; }

    SetColorIndex(board->GridColor);

    for (i = 1; i <= board->BoardWidth + 1; i++) {
        VerticalLine(i*cellWidth + offsetx, cellHeight + offsety, (board->BoardHeight) * cellHeight + 1);
    }

    for (i = 1; i <= board->BoardHeight + 1; i++) {
        HorizontalLine(cellWidth + offsetx, i * cellHeight + offsety, (board->BoardWidth) * cellWidth + 1);
    }
}