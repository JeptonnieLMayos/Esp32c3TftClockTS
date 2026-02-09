#include "clock_core.h"

static clock_time_t now;

void clock_core_init(void)
{
    now.hour = 0;
    now.minute = 0;
    now.second = 0;
}


void clock_core_tick(void)
{
    now.second++;
    if(now.second == 60)
    {
        now.second = 0;
        now.minute++;
        if(now.minute == 60)
        {
            now.minute = 0;
            now.hour = (now.hour + 1) % 12;
        }
    }
}

void clock_core_set(clock_time_t t)
{
    now = t;
}

clock_time_t clock_core_get(void)
{
    return now;
}
