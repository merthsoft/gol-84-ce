#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <tice.h>
#include <keypadc.h>
#include <string.h>

#include "include/key_helper.h"

#define NUM_KEY_ELEMENTS 8

uint16_t __previousKeys[NUM_KEY_ELEMENTS];
uint16_t __debounceKeys[NUM_KEY_ELEMENTS];

void Key_Init() {
    memset_fast(__previousKeys, 0, NUM_KEY_ELEMENTS);
}

void Key_ScanKeys(uint32_t keyDelay) {
    uint8_t i;
    uint8_t j;
    uint32_t currentTime = keyDelay;

    memcpy(__previousKeys, kb_DataArray, NUM_KEY_ELEMENTS * sizeof(uint16_t));
    if (keyDelay > 0) {
        memset_fast(__debounceKeys, 0, NUM_KEY_ELEMENTS);
        do {
            kb_Scan(); 
            for (j = 0; j < NUM_KEY_ELEMENTS; j++) {
                __debounceKeys[j] |= kb_Data[j];
            }
            currentTime--;
        } while (currentTime > 0);

        for (j = 0; j < NUM_KEY_ELEMENTS; j++) {
            kb_DataArray[j] = __debounceKeys[j];
        }
    } else {
        kb_Scan();
    }
}

bool Key_IsDown(kb_lkey_t key) {
    uint8_t group = (key & 0xFF00) >> 8;
    uint8_t code = key & 0x00FF;

    return kb_Data[group] & (code);
}

bool Key_WasDown(kb_lkey_t key) {
    uint8_t group = (key & 0xFF00) >> 8;
    uint8_t code = key & 0x00FF;

    return __previousKeys[group] & (code);
}

bool Key_JustPressed(kb_lkey_t key) {
    return !Key_WasDown(key) && Key_IsDown(key);
}

void Key_Reset() {
    kb_Reset(); 
}