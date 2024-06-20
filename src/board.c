#include <stdint.h>
#include <stdlib.h>
#include <tice.h>
#include <string.h>

#include "include/board.h"
#include "include/draw.h"

#define MOD_CYCLE(n, c) (((n) + (c)) % (c))

uint8_t plane_cell(uint8_t* c, uint8_t w, uint8_t h, uint8_t i, uint8_t j) {
    return c[(i) * (w+2) + (j)];
}

uint8_t torus_cell(uint8_t* cells, uint8_t w, uint8_t h, uint8_t c, uint8_t r) {
    uint8_t lC = 0;
    uint8_t lR = 0;

    bool lE = c == 0;
    bool tEE = r == 0;
    bool rE = c > w;
    bool bE = r > h;
    bool ovC = lE || rE;
    bool ovR = tEE || bE;

    if (!ovC && !ovR)
        return cells[(c) * (w+2) + (r)];

    lC = ovC ? (MOD_CYCLE(c - 1, w) + 1) : c;
    lR = ovR ? (MOD_CYCLE(r - 1, h) + 1) : r;
    
    return cells[(lC) * (w+2) + (lR)];
}

uint8_t ring_cell(uint8_t* cells, uint8_t w, uint8_t h, uint8_t c, uint8_t r) {
    uint8_t lC = 0;
    uint8_t lR = 0;

    bool lE = c == 0;
    bool tEE = r == 0;
    bool rE = c > w;
    bool bE = r > h;
    bool ovC = lE || rE;
    bool ovR = tEE || bE;

    if (!ovC && !ovR)
        return cells[(c) * (w+2) + (r)];

    lC = ovC ? (MOD_CYCLE(c - 1, w) + 1) : c;
    lR = r;
    
    return cells[(lC) * (w+2) + (lR)];
}

uint8_t mobius_cell(uint8_t* cells, uint8_t w, uint8_t h, uint8_t c, uint8_t r) {
    uint8_t lC = 0;
    uint8_t lR = 0;

    bool lE = c == 0;
    bool tEE = r == 0;
    bool rE = c > w;
    bool bE = r > h;
    bool ovC = lE || rE;
    bool ovR = tEE || bE;

    if (!ovC && !ovR)
        return cells[(c) * (w+2) + (r)];

    lC = ovC ? (MOD_CYCLE(c - 1, w) + 1) : c;
    lR = ovC ? (h - r + 2) : r;
    
    return cells[(lC) * (w+2) + (lR)];
}

uint8_t sphere_cell(uint8_t* cells, uint8_t w, uint8_t h, uint8_t c, uint8_t r) {
    uint8_t lC = 0;
    uint8_t lR = 0;

    bool lE = c == 0;
    bool tEE = r == 0;
    bool rE = c > w;
    bool bE = r > h;
    bool ovC = lE || rE;
    bool ovR = tEE || bE;

    if (!ovC && !ovR)
        return cells[(c) * (w+2) + (r)];

    /* This only makes sense if the board is square. Otherwise, all hell will break loose */
    uint8_t mask = ((uint8_t)lE << 3) | ((uint8_t)rE << 2) | ((uint8_t)tEE << 1) | ((uint8_t)bE);
    switch (mask) {
        case 0xA:
        case 0x5:
            return 0;
        case 0x9:
            lC = w;
            lR = 1;
            break;
        case 0x8:
            lC = r;
            lR = 1;
            break;
        case 0x6:
            lC = 1;
            lR = h;
            break;
        case 0x4:
            lC = r;
            lR = h;
            break;
        case 0x2:
            lC = 1;
            lR = c;
            break;
        case 0x1:
            lC = w;
            lR = c;
            break;
        default:
            lC = c;
            lR = r;
            break;
    }
    return cells[(lC) * (w+2) + (lR)];
}

uint8_t klein_cell(uint8_t* cells, uint8_t w, uint8_t h, uint8_t c, uint8_t r) {
    uint8_t lC = 0;
    uint8_t lR = 0;

    bool lE = c == 0;
    bool tEE = r == 0;
    bool rE = c > w;
    bool bE = r > h;
    bool ovC = lE || rE;
    bool ovR = tEE || bE;

    if (!ovC && !ovR)
        return cells[(c) * (w+2) + (r)];

    lC = ovC ? (MOD_CYCLE(c - 1, w) + 1) : c;
    lR = ovR ? (MOD_CYCLE(r - 1, w) + 1) : r;
    lR = ovC ? (h - lR + 2) : lR;
    
    return cells[(lC) * (w+2) + (lR)];
}

uint8_t proj_cell(uint8_t* cells, uint8_t w, uint8_t h, uint8_t c, uint8_t r) {
    uint8_t lC = 0;
    uint8_t lR = 0;

    bool lE = c == 0;
    bool tEE = r == 0;
    bool rE = c > w;
    bool bE = r > h;
    bool ovC = lE || rE;
    bool ovR = tEE || bE;

    if (!ovC && !ovR)
        return cells[(c) * (w+2) + (r)];

    
    lC = ovC ? (MOD_CYCLE(c - 1, w) + 1) : c;
    lC = ovR ? (w - lC + 2) : lC;
    lR = ovR ? (MOD_CYCLE(r - 1, h) + 1) : r;
    lR = ovC ? (h - lR + 2) : lR;
    
    return cells[(lC) * (w+2) + (lR)];
}

void* cell_func_dictionary[] = {
    plane_cell,
    ring_cell,
    mobius_cell,
    torus_cell,
    sphere_cell,
    klein_cell,
    proj_cell
};

void Step(Board* board) {
    uint8_t numN; // The number of neighbors
    uint8_t i, j;
    bool cellStatus;
    uint8_t boardNum = board->BoardNumber;
    Rules currentRule = board->Rules;
    uint8_t cellWidth = board->CellWidth;
    uint8_t cellHeight = board->CellHeight;
    uint8_t offsetX = board->OffsetX;
    uint8_t offsetY = board->OffsetY;
    uint8_t width = board->BoardWidth;
    uint8_t height = board->BoardHeight;
    uint8_t drawWidth = (width + 1) * cellWidth;
    uint8_t drawHeight = (height + 1) * cellHeight;
    uint8_t* cells = board->Cells[board->BoardNumber];
    uint8_t wrappingMode = board->WrappingMode;

    gfx_SetDrawBuffer();
    DrawRectFill(offsetX, offsetY,  drawWidth, drawHeight, board->DeadColor);
    DrawGrid(board);

    uint8_t (*cellFunc)(uint8_t* cells, uint8_t w, uint8_t h, uint8_t c, uint8_t r)
        = cell_func_dictionary[wrappingMode];

    for (i = 1; i <= board->BoardWidth; i++) {
        for (j = 1; j <= board->BoardHeight; j++) {
            uint16_t cell = i * (width+2) + j;
            // Count the 8 cells around it
            numN = cellFunc(cells, width, height, i - 1, j - 1)
                 + cellFunc(cells, width, height, i, j - 1) 
                 + cellFunc(cells, width, height, i + 1, j - 1) 
                 + cellFunc(cells, width, height, i - 1, j) 
                 + cellFunc(cells, width, height, i + 1, j) 
                 + cellFunc(cells, width, height, i - 1, j + 1) 
                 + cellFunc(cells, width, height, i, j + 1) 
                 + cellFunc(cells, width, height, i + 1, j + 1);
            
            cellStatus = board->Cells[boardNum][cell];
            // Rules:
            if (!(currentRule.Live & (1 << numN))) {
                cellStatus = 0;
            }

            if ((currentRule.Born & (1 << numN)) && cellStatus == 0) {
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
        uint16_t cell = x * (board->BoardWidth+2) + y;
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
    uint16_t cell = board->CursorX * (board->BoardWidth+2) + board->CursorY;
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
                uint16_t cell = (x + i) * (width+2) + (y + j);
                board->Cells[board->BoardNumber][cell] = stamp->Cells[j][i];
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

void ClearBoard(Board* board) {
    uint16_t size = (board->BoardWidth + 2) * (board->BoardHeight + 2);
    memset(board->Cells[0], 0, size);
    memset(board->Cells[1], 0, size);
}

void FillBoard(Board* board) {
    uint8_t i, j;
    uint8_t width = board->BoardWidth + 2;
    uint8_t height = board->BoardHeight + 2;

    for (i = 1; i < width; i++) {
        for (j = 1; j < height; j++) {
            uint16_t cell = i * width + j;
            board->Cells[0][cell] = 1;
            board->Cells[1][cell] = 1;
        }
    }

    board->BoardNumber = 0;
}

void __allocCells(Board* b) {
    uint16_t size = (b->BoardWidth + 2) * (b->BoardHeight + 2);
    b->Cells[0] = malloc(sizeof(uint8_t*) * size);
    b->Cells[1] = malloc(sizeof(uint8_t*) * size);
}

void SquareCells(Board* board, uint8_t cellSize, uint8_t drawSize, uint8_t maxSize) {
    uint8_t newSize = drawSize / cellSize;
    if (newSize > maxSize) 
        newSize = maxSize;

    ResizeBoard(board, newSize, newSize);

    board->CellHeight = cellSize;
    board->CellWidth = cellSize;
}

void InitializeBoard(Board* b, uint8_t boardWidth, uint8_t boardHeight) {
    memset(b, 0, sizeof(Board));
    b->CursorX = 1;
    b->CursorY = 1;
    b->BoardWidth = boardWidth;
    b->BoardHeight = boardHeight;

    __allocCells(b);

    b->BoardNumber = 0;
    b->Rules.Live = 0;
    b->Rules.Born = 0;
    b->Rules.Name = NULL;

    b->IsInitialized = true;
}

void DeleteBoard(Board* b) {
    FreeCells(b);
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
    if (b->Cells[0] != NULL)
    {
        free(b->Cells[0]);
        b->Cells[0] = NULL;
    }

    if (b->Cells[1] != NULL)
    {
        free(b->Cells[1]);
        b->Cells[1] = NULL;
    }
}

void SetRules(Board* b, Rules* rules) {
    memcpy(&b->Rules, rules, sizeof(Rules));
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
            uint16_t cell = i * (width+2) + j;
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