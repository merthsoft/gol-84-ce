#include "include/const.h"
#include "include/board.h"
#include "include/stamp_picker.h"
#include "include/draw.h"
#include "include/menu.h"

void DrawStamp(MenuEventArgs* menuEventArgs);

Stamp* ChooseStamp(Board* board) {
    uint8_t i;
    Rules* rules = board->Rules;
    uint8_t numStamps = rules->NumStamps;
    Stamp* stamps = rules->Stamps;

    Menu* menu = CreateMenu(numStamps + 1, "Stamps");
    //menu->ExtraFunction = DrawStamp;
    menu->SelectionType = Single;
    menu->Tag = board;

    for (i = 0; i < numStamps; i++) {
        menu->Items[i].Name = stamps[i].Name;
        menu->Items[i].Tag = &(stamps[i]);
    }
    
    menu->Items[numStamps].Name = BackString;
    menu->Items[numStamps].Function = FUNCTION_BACK;

    DisplayMenu(menu);
    DeleteMenu(menu);
    return NULL;
}

void __drawStamp(Stamp* stamp, Board* mainBoard) {
    /*
    Board* board = CreateBoard(stamp->Width, stamp->Height);
    board->OffsetX = 150;
    board->OffsetY = 9;
    board->CellHeight = mainBoard->CellHeight;
    board->CellWidth = mainBoard->CellWidth;
    board->GridColor = mainBoard->GridColor;
    board->DeadColor = mainBoard->DeadColor;
    board->AliveColor = mainBoard->AliveColor;
    board->CursorDeadColor = mainBoard->CursorDeadColor;
    board->CursorAliveColor = mainBoard->CursorAliveColor;
    */
    DrawString(stamp->Name, 150, 0);
    /*
    DrawGrid(board);
    DrawBoard(board, true);
    DeleteBoard(board);
    */
}

void DrawStamp(MenuEventArgs* menuEventArgs) {
    __drawStamp(menuEventArgs->Item, menuEventArgs->Menu->Tag);
}