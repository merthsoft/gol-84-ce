#include <graphx.h>
#include <fileioc.h>
#include <keypadc.h>
#include <time.h>
#include <string.h> 

#include "include/draw.h"
#include "include/menu.h"

Menu* CreateMenu(uint8_t numItems, char* title) {
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
    menu->BackKey = kb_KeyDel;
    menu->CursorChar = 0x10;
    menu->Tag = NULL;
    menu->FillColor = 255;

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

    FillScreen(menu->FillColor);

    while(kb_AnyKey());
    bool keyPressed = false;
    clock_t clockOffset = 0;
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
                    case None:
                        break;
                }
            }
        }

        DrawChar(menu->CursorChar, 2, 3 + linePadding * y);

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
        
        // If no key is pressed, reset timer and variable for keeping track of if a key is held down.
        if (clockOffset == 0 || (!kb_AnyKey() && keyPressed)) {
            keyPressed = false;
            clockOffset = clock();
        }

        // If no key is being held, allow an input
        // If a key is being held, introduce a small delay between inputs. In this example the delay is
        // CLOCKS_PER_SEC / 32, but it can be changed to what feels most natural.
        if (kb_AnyKey() && (!keyPressed || clock() - clockOffset > CLOCKS_PER_SEC / 8)) {
            clockOffset = clock();
            old_y = y;

            if (kb_IsDown(kb_KeyUp)) { y = y == 1 ? menu->NumItems : y - 1; }
            else if (kb_IsDown(kb_KeyDown)) { y = y == menu->NumItems ? 1 : y + 1; }
            else if (kb_IsDown(kb_Key2nd) || kb_IsDown(kb_KeyEnter)) {
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
                            case None:
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
                    while(kb_AnyKey());

                    y = eventArgs->Index + 1;
                    frameNumber = eventArgs->FrameNumber;
                    menu = eventArgs->Menu;
                    back = eventArgs->Back;
                }
                FillScreen(menu->FillColor);
            } else if (kb_IsDown(menu->BackKey)) {
                y = 0;
                back = true;
                menu->SelectedItem = NULL;
            }

            // If this is the first instance of an input after a release (the key has not already been held
            // down for some time), wait for a longer delay to ensure the user wants to continue holding down
            // the key and creating more inputs. In this example the delay is CLOCKS_PER_SEC / 4, but it can
            // be changed to what feels most natural.
            if (!keyPressed) {
                while (clock() - clockOffset < CLOCKS_PER_SEC / 4 && kb_AnyKey()) {
                    kb_Scan();
                }
            }

            // Now we know that the user is holding down a key (If not, we'll reset keyPressed back to false
            // at the beginning of the loop.
            keyPressed = true;
            
            if (old_y != y) {
                DrawRectFill(0, 3 + linePadding * old_y, 10, 8, menu->FillColor);
            }
        }

        frameNumber++;
    }

    free(eventArgs);
    return y-1;
}