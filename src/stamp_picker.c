#include <string.h>

#include "include/const.h"
#include "include/board.h"
#include "include/stamp_picker.h"
#include "include/draw.h"
#include "include/menu.h"

#define XOffSet 75
#define YOffSet 2

void __drawStampEvent(MenuEventArgs* menuEventArgs) {
    uint8_t i, j;
    Board* board;
    Stamp* stamp = menuEventArgs->Item->Tag;
    Board* mainBoard = menuEventArgs->Menu->Tag;
    
    if (stamp == NULL) {
        DrawRectFill(XOffSet, YOffSet, SCREEN_WIDTH - XOffSet, SCREEN_HEIGHT - YOffSet, 255);
        return;
    }

    board = CreateBoard(stamp->Width, stamp->Height);
    board->OffsetX = XOffSet;
    board->OffsetY = YOffSet + 9;
    board->CellHeight = mainBoard->CellHeight;
    board->CellWidth = mainBoard->CellWidth;
    board->GridColor = mainBoard->GridColor;
    board->DeadColor = mainBoard->DeadColor;
    board->CursorAliveColor = mainBoard->CursorAliveColor;
    board->CursorDeadColor = mainBoard->CursorDeadColor;
    
    DrawString(stamp->Name, XOffSet, YOffSet);
    HorizontalLine(XOffSet, YOffSet + 9, MeasureString(stamp->Name));
    DrawGrid(board);
    DrawStamp(board, stamp);
    DeleteBoard(board);
}

Stamp* ChooseStamp(Board* board) {
    uint8_t i;
    Rules* rules = board->Rules;
    uint8_t numStamps = rules->NumStamps;
    Stamp* stamps = rules->Stamps;

    Menu* menu = CreateMenu(numStamps + 1, "Stamps");
    menu->ExtraFunction = __drawStampEvent;
    menu->Tag = board;

    for (i = 0; i < numStamps; i++) {
        menu->Items[i].Name = stamps[i].Name;
        menu->Items[i].Tag = &(stamps[i]);
        menu->Items[i].Function = FUNCTION_BACK;
    }
    
    menu->Items[numStamps].Name = "None";
    menu->Items[numStamps].Function = FUNCTION_BACK;

    DisplayMenu(menu);
    stamps = menu->SelectedItem == NULL ? NULL : menu->SelectedItem->Tag;
    DeleteMenu(menu);
    return stamps;
}