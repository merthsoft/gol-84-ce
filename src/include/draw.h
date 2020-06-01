#ifndef DRAW_H
#define DRAW_H

#include <stdint.h>
#include <graphx.h>
#include <tice.h>

#include "../gfx/gfx.h"

#define SCREEN_WIDTH    LCD_WIDTH
#define SCREEN_HEIGHT   LCD_HEIGHT

#define GraphicsBegin()                         gfx_Begin();
#define GraphicsEnd()                           gfx_End()

#define SetTextColor(foreground, background)    gfx_SetTextBGColor(background); gfx_SetTextFGColor(foreground)
#define DrawString(str, x, y)                   gfx_PrintStringXY(str, x, y)
#define DrawChar(c, x, y)                       gfx_SetTextXY(x, y); gfx_PrintChar(c)
#define MeasureString(str)                      gfx_GetStringWidth(str)

#define SetColorIndex(color_index)              gfx_SetColor(color_index)
#define FillScreen(color_index)                 gfx_FillScreen(color_index)

void DrawRectFill(uint24_t x, uint8_t y, uint24_t width, uint8_t height, uint8_t color_index);
void DrawRect(uint24_t x, uint8_t y, uint24_t width, uint8_t height, uint8_t color_index);

#define VerticalLine(x, y, l)                   gfx_VertLine(x, y, l);
#define HorizontalLine(x, y, l)                 gfx_HorizLine(x, y, l);

#define DrawSprite(sprite, x, y)                gfx_Sprite_NoClip(sprite, x, y)
#define DrawSpriteScaled(sprite, x, y, w, h)    gfx_ScaledSprite_NoClip(sprite, x, y, w, h)

#endif