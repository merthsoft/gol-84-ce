#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "include/stamp.h"

#define MAX_LENGTH 40

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

void DeleteStamp(Stamp* stamp) {
    for (uint8_t i = 0; i < stamp->Width; i++) {
        free(stamp->Cells[i]);
    }
    free(stamp->Cells);
}

// void SetCells(Stamp* stamp, const char* apgcode) {
//     uint8_t i;
//     uint16_t len = strnlen(apgcode, MAX_LENGTH);
//     const char* chars = "0123456789abcdefghijklmnopqrstuvwxyz";
    
//     var blank = 0;
//     var x = 0;
//     var y = 0;
//     var cells = []

//     for (i = 0; i < len; i++) {
//         if (apgcode[i] == "_") {
//             i += 1;
//             break;
//         }
//     }
    
//     for (i = i; i < apgcode.length; i++) {
//         char c = apgcode[i];
//         if (blank) {
//             x += chars.indexOf(c);
//             blank = 0;
//         } else if (c == 'y') {
//             x += 4;
//             blank = 1;
//         } else if (c == 'x') {
//             x += 3;
//         } else if (c == 'w') {
//             x += 2;
//         } else if (c == 'z') {
//             x = 0;
//             y += 5;
//         } else {
//             v = chars.indexOf(c);
//             for (var j = 0; j < 5; j++)
//                 if (v & (1 << j)) {
//                     cells.push(x);
//                     cells.push(y+j);
//                 }
//             x += 1;
//         }
//     }

//     return cells;
// }