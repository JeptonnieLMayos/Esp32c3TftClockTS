#include "input.h"
#include "esp_log.h"


static gesture_t state;
static bool touching = false;
static touch_point_t start;
static touch_point_t end;
static uint8_t dsc = 0;


void det(void)
{
    int x = end.x - start.x;
    int y = end.y - start.y;
    if(abs(x) > abs(y))
    {
        if(x > 10)
            state = GESTURE_SWIPE_DOWN;
        if(x < -10)
            state = GESTURE_SWIPE_UP;
    }
    else if(abs(y) > abs(x))
    {
        if(y > 10)
            state = GESTURE_SWIPE_LEFT;
        if(y < -10)
            state = GESTURE_SWIPE_RIGHT;
    }
    ESP_LOGI("TOU", " %d", state);
}

void input_init(void)
{
    state = GESTURE_NONE;
}

void input_update(void)
{
    if(touch_is_pressed())
    {
        if(!touching)
        {
            touching = true;
            touch_read(&start);
        }
        if(touching)
        {
            touch_read(&end);
        }
    }
    else
    {
        if(touching)
        {
            det();
            touching = false;
        }
    }
}

gesture_t input_get_gesture(void)
{
    return state;
}