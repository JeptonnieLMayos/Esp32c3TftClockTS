#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint16_t x;
    uint16_t y;
} touch_point_t;

void touch_init(void);
bool touch_read(touch_point_t *p);
bool touch_is_pressed(void);