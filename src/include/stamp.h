#ifndef STAMP_H
#define STAMP_H

#include <stdint.h>

typedef struct {
    uint8_t Width;
    uint8_t Height;
    uint8_t** Cells;
    char* Name;
} Stamp;

void InitStamp(Stamp* stamp, uint8_t width, uint8_t height);
void DeleteStamp(Stamp* stamp);

#endif