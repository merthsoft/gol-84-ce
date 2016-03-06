#include "draw.h"

void SetTextColor(uint8_t foreground, uint8_t background) {
	gc_SetTextColor(foreground | background << 8);
}

void DrawRectFill(int x, int y, int w, int h, uint8_t color_index) {
	gc_SetColorIndex(color_index);
	gc_NoClipRectangle(x, y, w, h);
}