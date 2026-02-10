#pragma once
#include <stdint.h>
void backlight_init(void);
void set_backlight_brightness(uint8_t bl_strength);
uint8_t get_backlight_brightness(void);