#ifndef DRAW_H
#define DRAW_H

#include <stdint.h>
#include <graphc.h>

void DrawRectFill(int x, int y, int w, int h, uint8_t color_index);
void SetTextColor(uint8_t, uint8_t);

#endif