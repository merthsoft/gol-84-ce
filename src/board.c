#include <stdint.h>
#include <stdlib.h>
#include <tice.h>
#include <string.h>

#include "include/board.h"
#include "include/draw.h"
#include "include/key_helper.h"

#define MOD_CYCLE(n, c) (((n) + (c)) % (c))

uint8_t WrapToBoard(uint8_t* cells, WrappingMode wrappingMode, uint8_t CYCLE_WIDTH, uint8_t CYCLE_HEIGHT, uint8_t c, uint8_t r) {
    if (wrappingMode == Plane)
        return cells[c * CYCLE_WIDTH + r];

    uint8_t lC = 0;
    uint8_t lR = 0;

    bool lE = c == 0;
    bool tEE = r == 0;
    bool rE = c > CYCLE_WIDTH;
    bool bE = r > CYCLE_HEIGHT;
    bool ovC = lE || rE;
    bool ovR = tEE || bE;

    if (!ovC && !ovR)
        return cells[c * CYCLE_WIDTH + r];

    switch (wrappingMode) {
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
    return cells[lC * CYCLE_WIDTH + lR];
}

#define quickCell(c, w, i, j) (c[(i) * (w) + (j)])

void Step(Board* board) {
    uint8_t numN; // The number of neighbors
    uint8_t i, j;
    bool cellStatus;
    uint8_t boardNum = board->BoardNumber;
    Rules* currentRule = board->Rules;
    uint8_t cellWidth = board->CellWidth;
    uint8_t cellHeight = board->CellHeight;
    uint8_t offsetX = board->OffsetX;
    uint8_t offsetY = board->OffsetY;
    uint8_t width = board->BoardWidth;
    uint8_t height = board->BoardHeight;
    uint8_t drawWidth = (width + 1) * cellWidth;
    uint8_t drawHeight = (height + 1) * cellHeight;
    uint8_t* cells = board->Cells[board->BoardNumber];
    WrappingMode wrappingMode = board->WrappingMode;

    gfx_SetDrawBuffer();
    DrawRectFill(offsetX, offsetY,  drawWidth, drawHeight, 255);
    DrawGrid(board);

    if (wrappingMode == Plane) {
        for (i = 1; i <= board->BoardWidth; i++) {
            for (j = 1; j <= board->BoardHeight; j++) {
                uint16_t cell = i * width + j;
                // Count the 8 cells around it
                numN = quickCell(cells, width, i - 1, j - 1)
                     + quickCell(cells, width, i, j - 1) 
                     + quickCell(cells, width, i + 1, j - 1) 
                     + quickCell(cells, width, i - 1, j) 
                     + quickCell(cells, width, i + 1, j) 
                     + quickCell(cells, width, i - 1, j + 1) 
                     + quickCell(cells, width, i, j + 1) 
                     + quickCell(cells, width, i + 1, j + 1);
                
                cellStatus = board->Cells[boardNum][cell];
                // Rules:
                if (!(currentRule->Live & (1 << numN))) {
                    cellStatus = 0;
                }

                if ((currentRule->Born & (1 << numN)) && cellStatus == 0) {
                    cellStatus = 1;
                }
                
                if (cellStatus)  {
                    if (cellWidth > 2) {
                        DrawRectFill(i*cellWidth + 1, j * cellHeight + 1, cellWidth - 1, cellHeight - 1, board->AliveColor);
                    } else {
                        DrawRectFill(i*cellWidth, j * cellHeight, cellWidth, cellHeight, board->AliveColor);
                    }
                }

                board->Cells[!boardNum][cell] = cellStatus;
            }
        }
    } else {
        for (i = 1; i <= board->BoardWidth; i++) {
            for (j = 1; j <= board->BoardHeight; j++) {
                uint16_t cell = i * width + j;
                // Count the 8 cells around it
                numN = WrapToBoard(cells, wrappingMode, width, height, i - 1, j - 1)
                    + WrapToBoard(cells, wrappingMode, width, height, i, j - 1) 
                    + WrapToBoard(cells, wrappingMode, width, height, i + 1, j - 1) 
                    + WrapToBoard(cells, wrappingMode, width, height, i - 1, j) 
                    + WrapToBoard(cells, wrappingMode, width, height, i + 1, j) 
                    + WrapToBoard(cells, wrappingMode, width, height, i - 1, j + 1) 
                    + WrapToBoard(cells, wrappingMode, width, height, i, j + 1) 
                    + WrapToBoard(cells, wrappingMode, width, height, i + 1, j + 1);
                
                cellStatus = board->Cells[boardNum][cell];
                // Rules:
                if (!(currentRule->Live & (1 << numN))) {
                    cellStatus = 0;
                }

                if ((currentRule->Born & (1 << numN)) && cellStatus == 0) {
                    cellStatus = 1;
                }
                
                if (cellStatus)  {
                    if (cellWidth > 2) {
                        DrawRectFill(i*cellWidth + 1, j * cellHeight + 1, cellWidth - 1, cellHeight - 1, board->AliveColor);
                    } else {
                        DrawRectFill(i*cellWidth, j * cellHeight, cellWidth, cellHeight, board->AliveColor);
                    }
                }

                board->Cells[!boardNum][cell] = cellStatus;
            }
        }
    }
    board->BoardNumber = !boardNum; // Use the new board as the current board

    gfx_BlitRectangle(gfx_buffer, offsetX, offsetY, drawWidth, drawHeight);
    gfx_SetDrawScreen();
}


void __colorCell(Board* board, uint8_t x, uint8_t y, uint8_t offsetx, uint8_t offsety, color c) {
    uint8_t cellWidth = board->CellWidth;
    uint8_t cellHeight = board->CellHeight;

    if (cellWidth > 2)
        DrawRectFill(x*cellWidth + 1 + offsetx, y * cellHeight + 1 + offsety, cellWidth - 1, cellHeight - 1, c);
    else
        DrawRectFill(x*cellWidth + offsetx, y * cellHeight + offsety, cellWidth, cellHeight, c);
}

void __drawStamp(Board* board, Stamp* stamp, bool clear) {
    uint8_t i, j;
    uint8_t offsetx = board->OffsetX;
    uint8_t offsety = board->OffsetY;
    uint8_t x = board->CursorX;
    uint8_t y = board->CursorY;
    uint8_t* cells = board->Cells[board->BoardNumber];
    uint8_t width = board->BoardWidth;
    uint8_t height = board->BoardHeight;

    for (i = 0; i < stamp->Width; i++) {
        for (j = 0; j < stamp->Height; j++) {
            if (x + i >= 1 && x + i <= width && y + j >= 1 && y + j <= height) {
                uint16_t cell = (x + i) * width + (y + j);
                color color = clear
                              ? cells[cell] 
                                  ? board->AliveColor
                                  : board->DeadColor
                              : stamp->Cells[i][j]
                                  ? cells[cell]
                                      ? board->CursorAliveColor
                                      : board->CursorDeadColor
                                  : board->DeadColor;
                __colorCell(board, x + i, y + j, offsetx, offsety, color);
            }
        }
    }
}

void DrawCursor(Board* board, Stamp* stamp, bool clear) {
    if (stamp != NULL)
        __drawStamp(board, stamp, clear);
    else {
        uint8_t offsetx = board->OffsetX;
        uint8_t offsety = board->OffsetY;
        uint8_t x = board->CursorX;
        uint8_t y = board->CursorY;
        uint16_t cell = x * board->BoardWidth + y;
        color color = clear
                      ? board->Cells[board->BoardNumber][cell]
                        ? board->AliveColor
                        : board->DeadColor
                      : board->Cells[board->BoardNumber][cell] 
                        ? board->CursorAliveColor 
                        : board->CursorDeadColor;
        __colorCell(board, x, y, offsetx, offsety, color);
    }
}

void ToggleCell(Board* board) {
    uint16_t cell = board->CursorX * board->BoardWidth + board->CursorY;
    board->Cells[board->BoardNumber][cell] = !board->Cells[board->BoardNumber][cell];
}

void PlaceStamp(Board* board, Stamp* stamp) {
    uint8_t i, j;
    uint8_t x = board->CursorX;
    uint8_t y = board->CursorY;
    uint8_t width = board->BoardWidth;
    uint8_t height = board->BoardHeight;

    for (i = 0; i < stamp->Width; i++) {
        for (j = 0; j < stamp->Height; j++) {
            if (x + i >= 1 && x + i <= width && y + j >= 1 && y + j <= height) {
                uint16_t cell = (x + i) * width + (y + j);
                board->Cells[board->BoardNumber][cell] = stamp->Cells[i][j];
            }
        }
    }
}

void RandomBoard(Board* board) {
    uint8_t i, j;
    uint8_t width = board->BoardWidth;
    uint8_t height = board->BoardHeight;
    srand(rtc_Time());

    for (i = 1; i <= width + 1; i++) {
        for (j = 1; j <= height + 1; j++) {
            uint16_t cell = i * width + j;
            bool alive = (rand() % 100) < board->RandomChance;
            board->Cells[0][cell] = alive;
            board->Cells[1][cell] = alive;
        }
    }

    board->BoardNumber = 0;
}

void __setValue(Board* board, uint8_t val) {
    uint8_t i, j;
    uint8_t width = board->BoardWidth;
    uint8_t height = board->BoardHeight;

    for (i = 1; i < width + 2; i++) {
        for (j = 1; j < height + 2; j++) {
            uint16_t cell = i * width + j;
            board->Cells[0][cell] = val;
            board->Cells[1][cell] = val;
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
    uint8_t i;

    for (i = 0; i < 2; i++) {
        b->Cells[i] = malloc(sizeof(uint8_t*) * (b->BoardWidth + 2) * (b->BoardHeight + 2));
    }
}

void SquareCells(Board* board, uint8_t cellSize, uint8_t drawSize, uint8_t maxSize) {
    uint8_t newSize = drawSize / cellSize;
    if (newSize > maxSize) 
        newSize = maxSize;

    ResizeBoard(board, newSize, newSize);

    board->CellHeight = cellSize;
    board->CellWidth = cellSize;
}

Board* CreateBoard(uint8_t boardWidth, uint8_t boardHeight) {
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

    if (b->Cells[0] == NULL)
        return;
    
    for (i = 0; i < 2; i++) {
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
    uint8_t width = board->BoardWidth;
    uint8_t height = board->BoardHeight;

    for (i = 1; i <= width; i++) {
        for (j = 1; j <= height; j++) {
            uint16_t cell = i * width + j;
            if (board->Cells[boardNumber][cell] != board->Cells[!boardNumber][cell] || redraw) {
                __colorCell(board, i, j, offsetx, offsety, board->Cells[boardNumber][cell] ? board->AliveColor : board->DeadColor);
            }
        }
    }
}

void DrawGrid(Board* board) {
    uint8_t i;
    uint8_t offsetx = board->OffsetX;
    uint8_t offsety = board->OffsetY;
    uint8_t cellWidth = board->CellWidth;
    uint8_t cellHeight = board->CellHeight;

    if (board->CellHeight <= 2)
        return;

    SetColorIndex(board->GridColor);

    for (i = 1; i <= board->BoardWidth + 1; i++)
        VerticalLine(i*cellWidth + offsetx, cellHeight + offsety, (board->BoardHeight) * cellHeight + 1);

    for (i = 1; i <= board->BoardHeight + 1; i++)
        HorizontalLine(cellWidth + offsetx, i * cellHeight + offsety, (board->BoardWidth) * cellWidth + 1);
}