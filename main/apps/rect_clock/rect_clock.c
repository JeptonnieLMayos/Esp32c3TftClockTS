#include "rect_clock.h"

volatile int adjust = 0;


void get_rect_point(
    float radians,
    int cx, int cy,
    int half_w, int half_h,
    int *out_x, int *out_y)
{
    float dx = cosf(radians);
    float dy = -sinf(radians);

    float tx = 1e9f;
    float ty = 1e9f;

    if (dx != 0.0f) {
        float x_edge = dx > 0 ? cx + half_w : cx - half_w;
        tx = (x_edge - cx) / dx;
    }

    if (dy != 0.0f) {
        float y_edge = dy > 0 ? cy + half_h : cy - half_h;
        ty = (y_edge - cy) / dy;
    }

    float t = tx < ty ? tx : ty;

    *out_x = cx + (int)(dx * t);
    *out_y = cy + (int)(dy * t);
}


void draw_hour_markers(uint16_t col)
{
    for (int i = 0; i < 12; ++i)
    {
        float angle_deg = i * 30.0f;
        float radians   = angle_deg * (M_PI / 180.0f);

        int ix, iy, ox, oy;
        
        int hwi = (i == 0 || i == 6 ) ? 100 : 102;
        int hwo = 110;
        int hhi = ( i == 3 || i == 9 ) ? 140 : 142;
        int hho = 150;




        get_rect_point(radians, 120, 160, hwi, hhi, &ix, &iy);
        get_rect_point(radians, 120, 160, hwo, hho, &ox, &oy);

        int thickness = i == 0 ? 10 : 6;
        uint16_t *b = NULL;
        gfx_draw_line(ix, iy, ox, oy, thickness, col, &b);
    }
}


void draw_minute_markers(uint16_t col)
{
    for (int i = 0; i < 60; ++i)
    {
        if ( i % 5 != 0)
        {

            float angle_deg = i * 6.0f;
            float radians   = angle_deg * (M_PI / 180.0f);
    
            int ix, iy, ox, oy;
    
            get_rect_point(radians, 120, 160, 109, 149, &ix, &iy);
            get_rect_point(radians, 120, 160, 110, 150, &ox, &oy);
    
            uint16_t *b = NULL;
            gfx_draw_line(ix, iy, ox, oy, 1, col, &b);
        }
    }
}


void draw_borders(void)
{

    uint16_t col = 0xFFFF;

        draw_minute_markers(col);
        draw_hour_markers(col);

        uint16_t *bu = NULL; uint16_t *bb = NULL; uint16_t *br = NULL; uint16_t *bl = NULL;

        gfx_draw_rect(0, 0, 5,319, col, &bu);
        gfx_draw_rect(239, 0, 234, 319, col, &bb);
        gfx_draw_rect(5, 0, 233, 5, col, &br);
        gfx_draw_rect(5, 314, 233, 319, col, &bl);
    
}


void rect_clock (void *arg)
{
    draw_borders();

    clock_core_init();
    clock_time_t now = clock_core_get();
    uint8_t hour = now.hour % 12;
    uint8_t minute = now.minute;
    uint8_t second = now.second;

    float angle_per_tick = 6.0f;
    float radians = M_PI / 180.0f;

    float sec_angle = second * angle_per_tick;
    float sec_radians = sec_angle * radians;
    float min_angle = minute * angle_per_tick;
    float min_radians = min_angle * radians;
    float hour_angle = (hour * 30) + (minute * 0.5);
    float hour_radians = hour_angle * radians;
    

    uint16_t col = 0xFFFF;

    bool first = true;
    int spix, spiy, spox, spoy;
    int mpix, mpiy, mpox, mpoy;
    int hpix, hpiy, hpox, hpoy;
    uint16_t *sec_b = NULL; uint16_t *prev_sec_b = NULL;
    uint16_t *min_b = NULL; uint16_t *prev_min_b = NULL;
    uint16_t *hour_b = NULL; uint16_t *prev_hour_b = NULL;


    while(1)
    {
        now = clock_core_get();
        hour = now.hour % 12;
        minute = now.minute;
        second = now.second;
        if( !first )
        {
            free(sec_b); free(prev_sec_b);
            free(min_b); free(prev_min_b);
            free(hour_b); free(prev_hour_b);
        }

        if ( !first )
        {
            gfx_draw_line( spix, spiy, spox, spoy, 2, 0x0000, &prev_sec_b );

            gfx_draw_line( mpix, mpiy, mpox, mpoy, 4, 0x0000, &prev_min_b );
            gfx_draw_line( hpix, hpiy, hpox, hpoy, 8, 0x0000, &prev_hour_b );
        }
        else
            first = false;

        sec_angle = second * angle_per_tick;
        sec_radians = sec_angle * radians;

        if (second == 0 || adjust != 0) // bug after adjust
        {
            min_angle = minute * angle_per_tick;
            min_radians = min_angle * radians;
            hour_angle = (hour * 30) + (minute * 0.5);
            hour_radians = hour_angle * radians;
        }

        int sec_ix = 95; int sec_iy = 135; int sec_ox = 98; int sec_oy = 138;
        int min_ix = 85; int min_iy = 125; int min_ox = 94; int min_oy = 134;
        int hour_ix = 70; int hour_iy = 110; int hour_ox = 84; int hour_oy = 124;

        int six, siy, sox, soy;
        int mix, miy, mox, moy;
        int hix, hiy, hox, hoy;

        get_rect_point(sec_radians, 120, 160, sec_ix, sec_iy, &six, &siy);
        get_rect_point(sec_radians, 120, 160, sec_ox, sec_oy, &sox, &soy);
        get_rect_point(min_radians, 120, 160, min_ix, min_iy, &mix, &miy);
        get_rect_point(min_radians, 120, 160, min_ox, min_oy, &mox, &moy);
        get_rect_point(hour_radians, 120, 160, hour_ix, hour_iy, &hix, &hiy);
        get_rect_point(hour_radians, 120, 160, hour_ox, hour_oy, &hox, &hoy);


        gfx_draw_line( six, siy, sox, soy, 2, col, &sec_b );
        gfx_draw_line( mix, miy, mox, moy, 4, col, &min_b );
        gfx_draw_line( hix, hiy, hox, hoy, 8, col, &hour_b );



        spix = six; spiy = siy; spox = sox; spoy = soy;
        mpix = mix; mpiy = miy; mpox = mox; mpoy = moy;
        hpix = hix; hpiy = hiy; hpox = hox; hpoy = hoy;

        int delay = 1000;

        if (adjust == 0)
        {
            delay = 1000;
            clock_core_tick();
        }
        else
        {
            if (adjust == 1 )
            {
                now.second = (second + 1) % 60;
                delay = 100;
            }
            if (adjust == 2 )
            {
                now.minute = (minute + 1) % 60;
                delay = 250;
            }
            if (adjust == 3 )
            {
                now.hour = (hour + 1) % 12;
                delay = 350;
            }
            clock_core_set(now);
        }

        vTaskDelay(pdMS_TO_TICKS(delay));
 
    }


}
