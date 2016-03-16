/* Keep these headers */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

/* Standard headers - it's recommended to leave them included */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <keypadc.h>

#include "key_helper.h"

uint16_t* __previousKeys; // This is in the .c so it doesn't clog up other files when you include the .h

void Key_Init() {
    __previousKeys = malloc(8 * sizeof(uint16_t));
}

bool Key_IsDown(key_t key) {
    uint8_t group = (key & 0xFF00) >> 8;
    uint8_t code = key & 0x00FF;

    return kb_dataArray[group] & (code);
}

bool Key_WasDown(key_t key) {
    uint8_t group = (key & 0xFF00) >> 8;
    uint8_t code = key & 0x00FF;

    return __previousKeys[group] & (code);
}

bool Key_JustPressed(key_t key) {
    return !Key_WasDown(key) && Key_IsDown(key);
}

void Key_ScanKeys(bool debounce) {
    int i = 0;
    memcpy(__previousKeys, kb_dataArray, 8*sizeof(uint16_t));
    if (debounce) {
        for (i = 0; i < 100; i++) { kb_Scan(); }
    } else {
        kb_Scan();
    }
}

void Key_Reset() { kb_Reset(); }