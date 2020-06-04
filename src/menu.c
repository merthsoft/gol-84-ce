#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "include/draw.h"
#include "include/key_helper.h"
#include "include/menu.h"

Menu* CreateMenu(uint8_t numItems, const char* title) {
    uint8_t i;

    Menu* menu = malloc(numItems * sizeof(Menu));
    memset(menu, 0, sizeof(Menu));
    menu->Items = malloc(numItems * sizeof(MenuItem));
    for (i = 0; i < numItems; i++) {
        menu->Items[i].Index = i;
        menu->Items[i].Function = FUNCTION_NONE;
        menu->Items[i].Name = "";
        menu->Items[i].Selected = false;
        menu->Items[i].Tag = NULL;
    }
    menu->NumItems = numItems;

    menu->Title = title;
    menu->ExtraFunction = FUNCTION_NONE;
    menu->BackKey = Key_Del;
    menu->CursorChar = 0x10;
    menu->Tag = NULL;

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
    uint32_t frameNumber = 0;
    MenuEventArgs* eventArgs;
    bool back = false;

    if (menu->SelectionType != None) {
        extraTextPadding = linePadding;
    }

    eventArgs = malloc(sizeof(MenuEventArgs));

    FillScreen(255);
    while (!back) {
        DrawString(menu->Title, 2, 1);
        SetColorIndex(0);
        HorizontalLine(1, 10, MeasureString(menu->Title) + 5);

        for (i = 0; i < menu->NumItems; i++) {
            DrawString(menu->Items[i].Name, textPadding + extraTextPadding, 3 + linePadding + linePadding * i);

            if (menu->SelectionType != None && menu->Items[i].Function != FUNCTION_BACK) {
                if (menu->Items[i].Selected) {
                    previouslySelectedIndex = i;
                    selected = true;
                } else {
                    selected = false;
                }

                switch (menu->SelectionType) {
                    case Single:
                        DrawSprite(selected ? radiobutton_filled : radiobutton_empty, textPadding, 3 + linePadding + linePadding * i - 1);
                        break;
                    case Multiple:
                        DrawSprite(selected ? checkbox_checked : checkbox_empty, textPadding, 3 + linePadding + linePadding * i - 1);
                        break;
                }
            }
        }

        DrawChar(menu->CursorChar, 2, 3 + linePadding * y);
        Key_ScanKeys(0);
        old_y = y;

        if (menu->ExtraFunction != FUNCTION_NONE) {
            void(*func)(MenuEventArgs*) = menu->ExtraFunction;
            eventArgs->FrameNumber = frameNumber;
            eventArgs->Menu = menu;
            eventArgs->Index = y - 1;
            eventArgs->Item = &(menu->Items[y - 1]);
            eventArgs->Back = false;

            func(eventArgs);
            y = eventArgs->Index + 1;
            frameNumber = eventArgs->FrameNumber;
            menu = eventArgs->Menu;
            back = eventArgs->Back;
        }

        if (Key_JustPressed(Key_Up)) { y = y == 1 ? menu->NumItems : y - 1; }
        else if (Key_JustPressed(Key_Down)) { y = y == menu->NumItems ? 1 : y + 1; }
        else if (Key_JustPressed(Key_2nd) || Key_JustPressed(Key_Enter)) {
            uint8_t index = y - 1;
            void(*func)(MenuEventArgs*) = menu->Items[index].Function;

            if (menu->SelectionType != None) {
                if (menu->Items[y - 1].Function != FUNCTION_BACK) {
                    switch (menu->SelectionType) {
                        case Single:
                            if (index != previouslySelectedIndex) {
                                menu->Items[previouslySelectedIndex].Selected = false;
                                menu->Items[index].Selected = true;
                                menu->SelectedItem = &menu->Items[index];
                            }
                            break;
                        case Multiple:
                            menu->Items[index].Selected = !menu->Items[index].Selected;
                            break;
                    }
                }
            } else {
                menu->SelectedItem = &menu->Items[index];
            }
            
            if (func == FUNCTION_BACK) 
                back = true;
            else if (func != FUNCTION_NONE) { 
                eventArgs->FrameNumber = frameNumber;
                eventArgs->Menu = menu;
                eventArgs->Index = index;
                eventArgs->Item = &(menu->Items[index]);
                eventArgs->Back = false;

                func(eventArgs);

                y = eventArgs->Index + 1;
                frameNumber = eventArgs->FrameNumber;
                menu = eventArgs->Menu;
                back = eventArgs->Back;
            }
            FillScreen(255);
        } else if (Key_JustPressed(menu->BackKey)) {
            y = 0;
            back = true;
            menu->SelectedItem = NULL;
        }

        if (old_y != y) {
            DrawRectFill(0, 3 + linePadding * old_y, 10, 8, 255);
        }

        frameNumber++;
    }

    free(eventArgs);
    return y-1;
}