#include "backlight.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

#define PIN_TFT_BL 1

#define BL_PWM_TIMER LEDC_TIMER_0
#define BL_PWM_MODE  LEDC_LOW_SPEED_MODE
#define BL_PWM_CH    LEDC_CHANNEL_0
#define BL_PWM_GPIO  PIN_TFT_BL
#define BL_PWM_FREQ  5000
#define BL_PWM_RES   LEDC_TIMER_8_BIT

uint8_t backlight_brightness = 50;

void backlight_init(void)
{
    ledc_timer_config_t timer = {
        .speed_mode = BL_PWM_MODE,
        .timer_num = BL_PWM_TIMER,
        .duty_resolution = BL_PWM_RES,
        .freq_hz = BL_PWM_FREQ,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer);

    ledc_channel_config_t ch = {
        .gpio_num = BL_PWM_GPIO,
        .speed_mode = BL_PWM_MODE,
        .channel = BL_PWM_CH,
        .timer_sel = BL_PWM_TIMER,
        .duty = (backlight_brightness * 255) / 100,
        .hpoint = 0
    };
    ledc_channel_config(&ch);
    ledc_update_duty(BL_PWM_MODE, BL_PWM_CH);
}

void set_backlight_brightness(uint8_t bl_strength)
{
    if (bl_strength > 100) bl_strength = 100;
    if (bl_strength < 5) bl_strength = 5;
    backlight_brightness = bl_strength;

    uint8_t duty = (bl_strength * 255) / 100;
    ledc_set_duty(BL_PWM_MODE, BL_PWM_CH, duty);
    ledc_update_duty(BL_PWM_MODE, BL_PWM_CH);
}

uint8_t get_backlight_brightness(void)
{
    return backlight_brightness;
}
