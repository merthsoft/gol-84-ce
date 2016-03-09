#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <stdlib.h>

#define BACK_FUNCTION (void*)0

typedef struct {
	char* name;
	void* function;
} menuItem;

int Menu(const char* title, menuItem items[], uint8_t numItems, void* extraFunction);

#endif MENU_H