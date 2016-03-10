#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stdlib.h>

#define FUNCTION_BACK (void*)1
#define FUNCTION_NONE (void*)0

typedef struct MenuItem {
	char* Name;
	void* Function;
	bool Selected;
} MenuItem;

typedef enum MenuSelectionType {
    None,
    Single,
    Multiple
} MenuSelectionType;

typedef struct Menu {
    char* Title;
    MenuItem* Items;
    uint8_t NumItems;
    void* ExtraFunction;
    MenuSelectionType SelectionType;
} Menu;

Menu* CreateMenu(uint8_t numItems, const char* title);
void DeleteMenu(Menu* menu);
int DisplayMenu(Menu* menu);

#endif MENU_H