/* Keep these headers */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

/* Standard headers - it's recommended to leave them included */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <debug.h>

#include "misc_settings.h"
#include "menu.h"
#include "board.h"

void CellSizeSettings(MenuEventArgs* menuEventArgs) {
    uint8_t i;
    uint8_t previous;
    Board* mainBoard = menuEventArgs->Menu->Tag;

    Menu* menu = CreateMenu(16, "Cell Size");
    menu->SelectionType = Single;

    menu->Items[0].Name = "1";
    menu->Items[1].Name = "2";
    menu->Items[2].Name = "3";
    menu->Items[3].Name = "4";
    menu->Items[4].Name = "5";
    menu->Items[5].Name = "6";
    menu->Items[6].Name = "7";
    menu->Items[7].Name = "8";
    menu->Items[8].Name = "9";
    menu->Items[9].Name = "10";
    menu->Items[10].Name = "11";
    menu->Items[11].Name = "12";
    menu->Items[12].Name = "13";
    menu->Items[13].Name = "14";
    menu->Items[14].Name = "15";
    menu->Items[15].Name = BackString;
    menu->Items[15].Function = FUNCTION_BACK;

    menu->Items[mainBoard->CellHeight - 1].Selected = true;
    previous = mainBoard->CellHeight - 1;

    menu->BackKey = Key_Del;
    DisplayMenu(menu);

    for (i = 0; i < menu->NumItems - 1; i++) {
        if (menu->Items[i].Selected && i != previous) {
            int newSize = 224 / (i + 1);
            if (newSize > 80) { newSize = 80; }

            ResizeBoard(mainBoard, newSize, newSize);

            mainBoard->CellHeight = i + 1;
            mainBoard->CellWidth = i + 1;
            break;
        }
    }

    DeleteMenu(menu);
}

void RandomPercentageSettings(MenuEventArgs* menuEventArgs) {
    uint8_t i;
    uint8_t previous;
    Board* mainBoard = menuEventArgs->Menu->Tag;

    Menu* menu = CreateMenu(10, "Cell Size");
    menu->SelectionType = Single;

    menu->Items[0].Name = "10";
    menu->Items[1].Name = "20";
    menu->Items[2].Name = "30";
    menu->Items[3].Name = "40";
    menu->Items[4].Name = "50";
    menu->Items[5].Name = "60";
    menu->Items[6].Name = "70";
    menu->Items[7].Name = "80";
    menu->Items[8].Name = "90";;
    menu->Items[9].Name = BackString;
    menu->Items[9].Function = FUNCTION_BACK;

    menu->Items[(mainBoard->RandomChance - 1) / 10].Selected = true;

    menu->BackKey = Key_Del;
    DisplayMenu(menu);

    for (i = 0; i < 9; i++) {
        if (menu->Items[i].Selected) {
            mainBoard->RandomChance = (i + 1) * 10;
            break;
        }
    }

    DeleteMenu(menu);
}