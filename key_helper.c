#include "key_helper.h"

bool Key_IsDown(key_t key) {
	uint8_t group = (key & 0xFF00) >> 8;
	uint8_t code = key & 0x00FF;

	return kb_dataArray[group] & (code);
}