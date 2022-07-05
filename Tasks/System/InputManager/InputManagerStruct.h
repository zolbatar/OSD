#pragma once
#include <inttypes.h>

struct Key
{
    char ascii;
    uint8_t lv_keycode;
    uint8_t ro_keycode;
    uint32_t keycode;
};
