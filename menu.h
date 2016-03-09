#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stdlib.h>

#define BACK_FUNCTION (void*)0

typedef struct MenuItem {
	char* Name;
	void* Function;
} MenuItem;

int Menu(const char* title, MenuItem items[], uint8_t numItems, void* extraFunction);

#endif MENU_H