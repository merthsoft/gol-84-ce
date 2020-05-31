#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>
#include <graphx.h>

#include "include/menu.h"
#include "include/key_helper.h"
#include "include/board.h"
#include "include/topo_settings.h"
#include "include/draw.h"
#include "include/const.h"

void DrawTopoSprite(MenuEventArgs* menuEventArgs);

void TopologySettings(MenuEventArgs* menuEventArgs) {
    int i;
    Board* mainBoard = menuEventArgs->Menu->Tag;

    Menu* menu = CreateMenu(NUM_WRAPPING_MODES + 1, "Topologies");
    menu->SelectionType = Single;
    menu->ExtraFunction = DrawTopoSprite;

    for (i = 0; i < NUM_WRAPPING_MODES; i++) {
        menu->Items[i].Name = WrappingModeNames[i];
    }
    menu->Items[NUM_WRAPPING_MODES].Name = BackString;
    menu->Items[NUM_WRAPPING_MODES].Function = FUNCTION_BACK;

    menu->Items[mainBoard->WrappingMode].Selected = true;

    for (i = 0; i < NUM_WRAPPING_MODES; i++) {
        menu->Items[i].Function = DrawTopoSprite;
    }

    DisplayMenu(menu);

    for (i = 0; i < NUM_WRAPPING_MODES; i++) {
        if (menu->Items[i].Selected) {
            mainBoard->WrappingMode = i;
            break;
        }
    }

    DeleteMenu(menu);
}

void DrawTopoSprite(MenuEventArgs* menuEventArgs) {
    gfx_sprite_t* sprite = NULL;

    switch (menuEventArgs->Index) {
        case 0:
            sprite = topo_plane;
            break;
        case 1:
            sprite = topo_ring;
            break;
        case 2:
            sprite = topo_mobius;
            break;
        case 3:
            sprite = topo_torus;
            break;
        case 4:
            sprite = topo_sphere;
            break;
        case 5:
            sprite = topo_klein;
            break;
        case 6:
            sprite = topo_proj;
            break;
        default:
            sprite = topo_plane;
            break;
    }

    DrawSpriteScaled(sprite, 150, 9, 9, 9);
}