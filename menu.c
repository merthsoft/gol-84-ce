#include <stdint.h>
#include <stdlib.h>

#include <graphc.h>
#include <keypadc.h>

#include "draw.h"
#include "key_helper.h"
#include "menu.h"
#include "menu_sprites.h"

Menu* CreateMenu(uint8_t numItems, const char* title) {
    uint8_t i;

    Menu* menu = malloc(numItems * sizeof(Menu));
    menu->Items = malloc(numItems * sizeof(MenuItem));
    for (i = 0; i < numItems; i++) {
        menu->Items[i].Function = FUNCTION_NONE;
        menu->Items[i].Name = "";
        menu->Items[i].Selected = false;
    }
    menu->NumItems = numItems;

    menu->Title = title;
    menu->ExtraFunction = FUNCTION_NONE;

    return menu;
}

void DeleteMenu(Menu* menu) {
    free(menu->Items);
    free(menu);
}

int DisplayMenu(Menu* menu) {
	uint8_t i;
	uint8_t y = 1;
	uint8_t old_y = 1;
    uint8_t linePadding = 10;
    uint8_t textPadding = 10;
    uint8_t extraTextPadding = 0;
    uint8_t previouslySelectedIndex = 0;
    bool selected = false;

    if (menu->SelectionType != None) {
        extraTextPadding = linePadding;
    }

	gc_FillScrn(255);
	while (true) {
		gc_PrintStringXY(menu->Title, 1, 1);

		for (i = 0; i < menu->NumItems; i++) {
			gc_PrintStringXY(menu->Items[i].Name, textPadding + extraTextPadding, linePadding + linePadding * i);

            if (menu->SelectionType != None && menu->Items[i].Function != FUNCTION_BACK) {
                if (menu->Items[i].Selected) {
                    previouslySelectedIndex = i;
                    selected = true;
                } else {
                    selected = false;
                }

                switch (menu->SelectionType) {
                    case Single:
                        gc_NoClipDrawSprite(selected ? radiobutton_filled : radiobutton_empty, textPadding, linePadding + linePadding * i - 1, 9, 9);
                        break;
                    case Multiple:
                        gc_NoClipDrawSprite(selected ? checkbox_checked : checkbox_empty, textPadding, linePadding + linePadding * i - 1, 9, 9);
                        break;
                }
            }
		}

		if (menu->ExtraFunction != FUNCTION_NONE) {
			void(*func)() = menu->ExtraFunction;
			func();
		}

		gc_PrintStringXY(">", 2, linePadding * y);
		for (i = 0; i < 100; i++) { kb_Scan(); }
		old_y = y;

		if (Key_IsDown(Key_Up)) { y = y == 1 ? menu->NumItems : y - 1; }
		else if (Key_IsDown(Key_Down)) { y = y == menu->NumItems ? 1 : y + 1; }
		else if (Key_IsDown(Key_2nd) || Key_IsDown(Key_Enter)) {
            uint8_t index = y - 1;
            void(*func)() = menu->Items[index].Function;

            if (menu->SelectionType != None && menu->Items[y - 1].Function != FUNCTION_BACK) {
                switch (menu->SelectionType) {
                    case Single:
                        if (index != previouslySelectedIndex) {
                            menu->Items[previouslySelectedIndex].Selected = false;
                            menu->Items[index].Selected = true;
                        }
                        break;
                    case Multiple:
                        menu->Items[index].Selected = !menu->Items[index].Selected;
                        break;
                }
            }
			
			if (func == FUNCTION_BACK) { return index; }
			else if (func != FUNCTION_NONE) { func(); }
			gc_FillScrn(255);
		}

		if (old_y != y) {
			DrawRectFill(0, linePadding * old_y, 8, 8, 255);
		}
	}

	return -1;
}