#include <stdint.h>

typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} clock_time_t;

void clock_core_init(void);
void clock_core_tick(void);

void clock_core_set(clock_time_t t);
clock_time_t clock_core_get(void);
