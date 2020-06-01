#include <stdlib.h>
#include <string.h>

#include "include/stamp.h"

#define MAX_LENGTH 20

void InitStamp(Stamp* stamp, uint8_t width, uint8_t height) {
    uint8_t i, mallocSize;
    
    stamp->Width = width;
    stamp->Height = height;

    stamp->Cells = malloc(sizeof(uint8_t) * width);
    for (i = 0; i < width; i++) {
        mallocSize = sizeof(uint8_t) * height;
        stamp->Cells[i] = malloc(mallocSize);
        memset(stamp->Cells[i], 0, mallocSize);
    }
}

void DeleteStamp(Stamp* stamp);