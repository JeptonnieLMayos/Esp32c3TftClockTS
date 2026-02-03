#include <stdint.h>
#include "backlight.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_ili9341.h"
#include "driver/gpio.h"

void graphics_init(void);


#pragma region Remove
// ------------------------
void display_init(void);
void display_fill_color(uint16_t color);
void display_draw_icon(int x, int y,
                       int w, int h,
                       const uint16_t *icon);


#pragma endregion //Remove

