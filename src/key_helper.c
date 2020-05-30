/* Keep these headers */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <tice.h>
#include <keypadc.h>
#include <string.h>

#include "include/key_helper.h"

uint16_t* __previousKeys;

#define NUM_KEY_ELEMENTS 8

void Key_Init() {
    __previousKeys = malloc(NUM_KEY_ELEMENTS * sizeof(uint16_t));
    memset(__previousKeys, 0, NUM_KEY_ELEMENTS);
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

void Key_ScanKeys(uint32_t keyDelay) {
    uint8_t i;
    uint8_t j;
    uint16_t* debounceKeys;
    uint32_t currentTime = keyDelay;

    memcpy(__previousKeys, kb_dataArray, NUM_KEY_ELEMENTS * sizeof(uint16_t));
    debounceKeys = malloc(NUM_KEY_ELEMENTS * sizeof(uint16_t));
    memset_fast(debounceKeys, 0, NUM_KEY_ELEMENTS);
    if (keyDelay > 0) {
        do {
            kb_Scan(); 
            for (j = 0; j < NUM_KEY_ELEMENTS; j++) {
                debounceKeys[j] |= kb_dataArray[j];
            }
            currentTime--;
        } while (currentTime > 0);

        for (j = 0; j < NUM_KEY_ELEMENTS; j++) {
            kb_dataArray[j] = debounceKeys[j];
        }
    } else {
        kb_Scan();
    }
    free(debounceKeys);
}

void Key_Reset() { 
    free(__previousKeys);
    kb_Reset(); 
}