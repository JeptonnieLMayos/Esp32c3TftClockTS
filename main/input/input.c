#include "input.h"
#include "esp_log.h"


static gesture_t state;
static bool touching = false;
static touch_point_t start;
static touch_point_t end;

void det(void)
{
    int x = end.x - start.x;
    int y = end.y - start.y;
    ESP_LOGI("TOU", "x: %d | y: %d", x, y);
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