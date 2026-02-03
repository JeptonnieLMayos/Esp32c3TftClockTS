#include <stdio.h>
#include "graphics.h"

#define PIN_TFT_CS 7
#define PIN_TFT_DC 8
#define PIN_TFT_RST 9

void graphics_init(void)
{
    backlight_init();
}
