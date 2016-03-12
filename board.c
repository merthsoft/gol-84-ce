#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include "board.h"
#include "rule.h"
#include "draw.h"

void Step(Board* board) {
	uint8_t numN; // The number of neighbors
	uint8_t i;
	uint8_t j;
	uint8_t boardNum = board->BoardNumber;
    Rule* currentRule = board->Rule;

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
			
			// Rules:
			if (!(currentRule->Live & (1 << numN))) {
				board->Cells[!boardNum][i][j] = 0;
			} else {
				board->Cells[!boardNum][i][j] = board->Cells[boardNum][i][j];
			}

			if ((currentRule->Born & (1 << numN)) && board->Cells[boardNum][i][j] == 0) {
				board->Cells[!boardNum][i][j] = 1;
			}
		}
	}
	board->BoardNumber = !boardNum; // Use the new board as the current board
}

uint8_t WrapToBoard(Board* board, uint8_t c, uint8_t r) {
	uint8_t CYCLE_WIDTH = board->BoardWidth - 1;
	uint8_t CYCLE_HEIGHT = board->BoardHeight - 1;

	uint8_t lC = 0;
	uint8_t lR = 0;

	bool lE = c == 0, rE = c > CYCLE_WIDTH,
		tE = r == 0, bE = r > CYCLE_HEIGHT,
		ovC = lE || rE,
		ovR = tE || bE;
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
		lR = ovC ? (board->BoardHeight - r) : r;
		break;
	case Torus:
		lC = ovC ? (MOD_CYCLE(c - 1, CYCLE_WIDTH) + 1) : c;
		lR = ovR ? (MOD_CYCLE(r - 1, CYCLE_HEIGHT) + 1) : r;
		break;
	case Sphere:
	{
		/* This only makes sense if the board is square. Otherwise, all hell will break loose */
		uint8_t mask = ((uint8_t)lE << 3) | ((uint8_t)rE << 2) | ((uint8_t)tE << 1) | ((uint8_t)bE);
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
		lR = ovC ? (board->BoardHeight - lR) : lR;
		break;
	case Proj:
		lC = ovC ? (MOD_CYCLE(c - 1, CYCLE_WIDTH) + 1) : c;
		lC = ovR ? (board->BoardWidth + 2 - lC) : lC;
		lR = ovR ? (MOD_CYCLE(r - 1, CYCLE_HEIGHT) + 1) : r;
		lR = ovC ? (board->BoardHeight - lR) : lR;
		break;
	}
	return board->Cells[board->BoardNumber][lC][lR];
}

void DrawCell(Board* board, uint8_t x, uint8_t y) {
	uint8_t cellWidth = board->CellWidth;
	uint8_t cellHeight = board->CellHeight;

	DrawRectFill(x*cellWidth + 1, y * cellHeight + 1, cellWidth - 1, cellHeight - 1, board->Cells[board->BoardNumber][x][y] ? board->AliveColor : board->DeadColor);
}

void DrawCursor(Board* board, uint8_t x, uint8_t y, uint8_t offsetx, uint8_t offsety) {
	uint8_t cellWidth = board->CellWidth;
	uint8_t cellHeight = board->CellHeight;

	DrawRectFill(x*cellWidth + 1 + offsetx, y * cellHeight + 1 + offsety, cellWidth - 1, cellHeight - 1, board->Cells[board->BoardNumber][x][y] ? board->CursorAliveColor : board->CursorDeadColor);
}

void SetupBoard(Board* board) {
	uint8_t i, j;
	srand(rtc_Time());

	for (i = 0; i <= board->BoardWidth; i++) {
		for (j = 0; j <= board->BoardHeight; j++) {
			board->Cells[0][i][j] = !(rand() % board->RandomMod);
			board->Cells[1][i][j] = 0;
		}
	}

	board->BoardNumber = 0;
}

void ClearBoard(Board* board) {
	uint8_t i, j;
	for (i = 0; i < board->BoardWidth + 2; i++) {
		for (j = 0; j < board->BoardHeight + 2; j++) {
			board->Cells[0][i][j] = 0;
			board->Cells[1][i][j] = 0;
		}
	}

	board->BoardNumber = 0;
}

Board* CreateBoard(uint8_t boardWidth, uint8_t boardHeight) {
	int i;
	int j;
	Board* b = malloc(sizeof(Board));
	b->BoardWidth = boardWidth;
	b->BoardHeight = boardHeight;

	for (i = 0; i < 2; i++) {
		b->Cells[i] = malloc(8 * (boardWidth+2));
		for (j = 0; j < boardWidth+2; j++) {
			b->Cells[i][j] = malloc(8 * (boardHeight+2));
		}
	}

	b->BoardNumber = 0;
    b->Rule = malloc(sizeof(Rule));
    b->Rule->Live = 0;
    b->Rule->Born = 0;
    b->Rule->Name = NULL;
	return b;
}

void SetRule(Board* b, Rule* rule) {
    b->Rule->Live = rule->Live;
    b->Rule->Born = rule->Born;
    b->Rule->Name = rule->Name;
}

void DrawBoard(Board* board, bool redraw, uint8_t offsetx, uint8_t offsety) {
	uint8_t i, j;
	uint8_t boardNumber = board->BoardNumber;
	uint8_t cellWidth = board->CellWidth;
	uint8_t cellHeight = board->CellHeight;

	for (i = 1; i <= board->BoardWidth; i++) {
		for (j = 1; j <= board->BoardHeight; j++) {
			if (board->Cells[boardNumber][i][j] != board->Cells[!boardNumber][i][j] || redraw) {
				DrawRectFill(i*cellWidth + 1 + offsetx, j * cellHeight + 1 + offsety, cellWidth - 1, cellHeight - 1, board->Cells[boardNumber][i][j] ? board->AliveColor : board->DeadColor);
			}
		}
	}
}

void DrawGrid(Board* board, uint8_t offsetx, uint8_t offsety) {
	uint8_t i;
	uint8_t cellWidth = board->CellWidth;
	uint8_t cellHeight = board->CellHeight;

	gc_SetColorIndex(board->GridColor);

	for (i = 1; i <= board->BoardWidth + 1; i++) {
		gc_NoClipVertLine(i*cellWidth + offsetx, cellHeight + offsety, (board->BoardHeight) * cellHeight + 1);
	}

	for (i = 1; i <= board->BoardHeight + 1; i++) {
		gc_NoClipHorizLine(cellWidth + offsetx, i * cellHeight + offsety, (board->BoardWidth) * cellWidth + 1);
	}
}