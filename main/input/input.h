#pragma once
#include <stdlib.h>
#include "touch.h"

typedef enum {
    GESTURE_NONE,
    GESTURE_TAP,
    GESTURE_DOUBLE_TAP,
    GESTURE_LONG_PRESS,
    GESTURE_SWIPE_LEFT,
    GESTURE_SWIPE_RIGHT,
    GESTURE_SWIPE_UP,
    GESTURE_SWIPE_DOWN
} gesture_t;

void input_init(void);
void input_update(void);
gesture_t input_get_gesture(void);