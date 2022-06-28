#pragma once
#include <inttypes.h>

struct Key {
	char ascii;
	uint32_t keycode;
	uint8_t ro_keycode;
};

