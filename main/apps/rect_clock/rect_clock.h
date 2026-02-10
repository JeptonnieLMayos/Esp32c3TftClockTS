#pragma once
#include <stdint.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "clock_core.h"
#include "graphics.h"

extern volatile int adjust;

void rect_clock (void *arg);