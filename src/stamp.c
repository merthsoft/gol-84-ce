#include <stdlib.h>
#include <string.h>

#include "include/stamp.h"

#define MAX_LENGTH 20

void InitStamp(Stamp* stamp, uint8_t width, uint8_t height) {
    uint8_t i;
    
    stamp->Width = width;
    stamp->Height = height;

    stamp->Cells = malloc(sizeof(uint8_t) * width);
    for (i = 0; i < width; i++) {
        stamp->Cells[i] = malloc(sizeof(uint8_t) * height);
    }
}

void DeleteStamp(Stamp* stamp);