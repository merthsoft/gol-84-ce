#include "include/draw.h"

void DrawRectFill(uint24_t x, uint8_t y, uint24_t width, uint8_t height, color color_index)
{
    gfx_SetColor(color_index); 
    gfx_FillRectangle_NoClip(x, y, width, height);
}

void DrawRect(uint24_t x, uint8_t y, uint24_t width, uint8_t height, color color_index)
{
    gfx_SetColor(color_index); 
    gfx_Rectangle_NoClip(x, y, width, height);
}