#ifndef DRAW_H
#define DRAW_H

#include <graphx.h>
#include "../gfx/gfx.h"

#define GraphicsBegin()                         gfx_Begin()
#define GraphicsEnd()                           gfx_End()

#define SetTextColor(foreground, background)    gfx_SetTextBGColor(background); gfx_SetTextFGColor(foreground)
#define DrawString(str, x, y)                   gfx_PrintStringXY(str, x, y)
#define DrawChar(c, x, y)                       gfx_SetTextXY(x, y); gfx_PrintChar(c)
#define MeasureString(str)                      gfx_GetStringWidth(str)

#define SetColorIndex(color_index)              gfx_SetColor(color_index)
#define FillScreen(color_index)                 gfx_FillScreen(color_index)

#define DrawRectFill(x, y, w, h, color_index)   gfx_SetColor(color_index); gfx_FillRectangle(x, y, w, h)
#define DrawRect(x, y, w, h, color_index)       gfx_SetColor(color_index); gfx_Rectangle(x, y, w, h)

#define VerticalLine(x, y, l)                   gfx_VertLine(x, y, l);
#define HorizontalLine(x, y, l)                 gfx_HorizLine(x, y, l);

#define DrawSprite(sprite, x, y)                gfx_Sprite_NoClip(sprite, 130, 110)

#endif