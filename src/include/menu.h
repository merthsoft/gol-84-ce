#ifndef MENU_H
#define MENU_H

#include <keypadc.h>

#define FUNCTION_BACK (void*)1
#define FUNCTION_NONE (void*)0

typedef uint8_t color;

typedef struct MenuItem {
    int Index;
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
    MenuItem* SelectedItem;
    kb_lkey_t BackKey;
    char CursorChar;
    void* Tag;
    color FillColor;
} Menu;

typedef struct MenuEventArgs {
    Menu* Menu;
    uint8_t Index;
    MenuItem* Item;
    uint32_t FrameNumber;
    bool Back;
} MenuEventArgs;

Menu* CreateMenu(uint8_t numItems, char* title);
void DeleteMenu(Menu* menu);
int DisplayMenu(Menu* menu);

#endif