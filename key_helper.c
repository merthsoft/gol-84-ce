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

#include <lib/ce/keypadc.h>

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

void Key_ScanKeys(uint32_t keyDelayMs) {
    uint8_t i;
    uint8_t j;
    uint16_t* debounceKeys;
    uint32_t currentTime = keyDelayMs;

    memcpy(__previousKeys, kb_dataArray, 8*sizeof(uint16_t));
    debounceKeys = malloc(8 * sizeof(uint16_t));
    memset_fast(debounceKeys, 0, 8);
    if (keyDelayMs > 0) {
        do {
            kb_Scan(); 
            for (j = 0; j < 8; j++) {
                debounceKeys[j] |= kb_dataArray[j];
            }
            currentTime--;
        } while (currentTime > 0);

        for (j = 0; j < 8; j++) {
            kb_dataArray[j] = debounceKeys[j];
        }
        free(debounceKeys);
    } else {
        kb_Scan();
    }
}

void Key_Reset() { 
    free(__previousKeys);
    kb_Reset(); 
}