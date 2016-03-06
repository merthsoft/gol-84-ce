#include "board.h"

void Step() {
	uint8_t numN; // The number of neighbors
	uint8_t i;
	uint8_t j;

	for (i = 1; i < BOARD_WIDTH - 1; i++) {
		for (j = 1; j < BOARD_HEIGHT - 1; j++) {
			numN = 0;
			// Count the 8 cells around it
			numN += WrapToBoard(board, i - 1, j - 1, boardNumber, wrapMode)
				+ WrapToBoard(board, i, j - 1, boardNumber, wrapMode) + WrapToBoard(board, i + 1, j - 1, boardNumber, wrapMode) +
				WrapToBoard(board, i - 1, j, boardNumber, wrapMode) + WrapToBoard(board, i + 1, j, boardNumber, wrapMode) +
				WrapToBoard(board, i - 1, j + 1, boardNumber, wrapMode) + WrapToBoard(board, i, j + 1, boardNumber, wrapMode) + WrapToBoard(board, i + 1, j + 1, boardNumber, wrapMode);
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

uint8_t WrapToBoard(Board board, uint8_t c, uint8_t r, uint8_t layer, WrappingMode wrapRule) {
	uint8_t lC, lR;
	bool lE = c == 0, rE = c > CYCLE_WIDTH,
		tE = r == 0, bE = r > CYCLE_HEIGHT,
		ovC = lE || rE,
		ovR = tE || bE;
	switch (wrapRule) {
	case Plane:
		lC = c; lR = r;
		break;
	case Ring:
		lC = ovC ? (MOD_CYCLE(c - 1, CYCLE_WIDTH) + 1) : c;
		lR = r;
		break;
	case Mobius:
		lC = ovC ? (MOD_CYCLE(c - 1, CYCLE_WIDTH) + 1) : c;
		lR = ovC ? (BOARD_HEIGHT - r) : r;
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
		lR = ovC ? (BOARD_HEIGHT - lR) : lR;
		break;
	case Proj:
		lC = ovC ? (MOD_CYCLE(c - 1, CYCLE_WIDTH) + 1) : c;
		lC = ovR ? (BOARD_WIDTH - lC) : lC;
		lR = ovR ? (MOD_CYCLE(r - 1, CYCLE_HEIGHT) + 1) : r;
		lR = ovC ? (BOARD_HEIGHT - lR) : lR;
		break;
	}
	return board[lC][lR][layer];
}

void SetupBoard() {
	uint8_t i, j;
	srand(*(unsigned long*)0xF30044);

	for (i = 1; i < BOARD_WIDTH - 1; i++) {
		for (j = 1; j < BOARD_HEIGHT - 1; j++) {
			board[i][j][boardNumber] = !(rand() % randMod);
			board[i][j][!boardNumber] = 0;
		}
	}
}

void ClearBoard() {
	uint8_t i, j;
	for (i = 0; i < BOARD_WIDTH + 1; i++) {
		for (j = 0; j < BOARD_HEIGHT + 1; j++) {
			board[i][j][boardNumber] = 0;
			board[i][j][!boardNumber] = 0;
		}
	}
}

void DrawBoard(bool redraw) {
	uint8_t i, j;

	//Debug_PrintLine("Drawing board.");
	for (i = 1; i < BOARD_WIDTH - 1; i++) {
		for (j = 1; j < BOARD_HEIGHT - 1; j++) {
			if (board[i][j][boardNumber] != board[i][j][!boardNumber] || redraw) {
				DrawRectFill(i*CELL_WIDTH + 1, j * CELL_HEIGHT + 1, CELL_WIDTH - 1, CELL_HEIGHT - 1, board[i][j][boardNumber] ? aliveColor : deadColor);
			}
		}
	}
	//Debug_PrintLine("Board drawn.");

}

void DrawGrid() {
	uint8_t i;
	gc_SetColorIndex(gridColor);

	for (i = 1; i < BOARD_WIDTH; i++) {
		gc_NoClipVertLine(i*CELL_WIDTH, CELL_HEIGHT, (BOARD_HEIGHT - 2) * CELL_HEIGHT + 1);
	}

	for (i = 1; i < BOARD_HEIGHT; i++) {
		gc_NoClipHorizLine(CELL_HEIGHT, i * CELL_HEIGHT, (BOARD_WIDTH - 2) * CELL_WIDTH + 1);
	}
}