#ifndef DRAW_H
#define DRAW_H

#include <stdint.h>
#include <lib/ce/graphc.h>

#define SetTextColor(foreground, background)  gc_SetTextColor(foreground | background << 8);

#define DrawRectFill(x, y, w, h, color_index) gc_SetColorIndex(color_index); gc_NoClipRectangle(x, y, w, h);

#endif