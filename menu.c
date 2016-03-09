#include <stdint.h>
#include <stdlib.h>

#include <graphc.h>
#include <keypadc.h>

#include "draw.h"
#include "key_helper.h"
#include "menu.h"

int Menu(const char* title, menuItem items[], uint8_t numItems, void* extraFunction) {
	uint8_t i;
	uint8_t y = 1;
	uint8_t old_y = 1;

	gc_FillScrn(255);
	while (!Key_IsDown(Key_Del)) {
		gc_PrintStringXY(title, 1, 1);

		for (i = 0; i < numItems; i++) {
			gc_PrintStringXY(items[i].name, 9, 9 + 9 * i);
		}
		if (extraFunction != NULL) {
			void(*func)() = extraFunction;
			func();
		}

		gc_PrintStringXY(">", 0, 9 * y);
		for (i = 0; i < 100; i++) { kb_Scan(); }
		old_y = y;

		if (Key_IsDown(Key_Up)) { y = y == 1 ? numItems : y - 1; }
		else if (Key_IsDown(Key_Down)) { y = y == numItems ? 1 : y + 1; }
		else if (Key_IsDown(Key_2nd) || Key_IsDown(Key_Enter)) {
			void (*func)() = items[y - 1].function;
			if (func == BACK_FUNCTION) { return y; }
			else { func(); }
			gc_FillScrn(255);
		}

		if (old_y != y) {
			DrawRectFill(0, 9 * old_y, 8, 8, 255);
		}
	}

	return -1;
}