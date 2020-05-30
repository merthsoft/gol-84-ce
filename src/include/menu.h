#ifndef MENU_H
#define MENU_H

#include "key_helper.h"

#define FUNCTION_BACK (void*)1
#define FUNCTION_NONE (void*)0

typedef struct MenuItem {
    char* Name;
    void* Function;
    bool Selected;
    void* Tag;
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
    kb_key_t BackKey;
    char CursorChar;
    void* Tag;
} Menu;

typedef struct MenuEventArgs {
    Menu* Menu;
    uint8_t Index;
    uint32_t FrameNumber;
    bool Back;
} MenuEventArgs;

Menu* CreateMenu(uint8_t numItems, const char* title);
void DeleteMenu(Menu* menu);
int DisplayMenu(Menu* menu);

#endif MENU_H