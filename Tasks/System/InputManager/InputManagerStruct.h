#pragma once
#include <inttypes.h>

struct Key
{
    char ascii;
    uint8_t lv_keycode;
    uint8_t ro_keycode;
    uint16_t keycode;
    bool still_pressed = true;
    bool first_key = true;
    unsigned int last_key = 0;
};
