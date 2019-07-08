#include "splashscreen.h"

BMAP * splashscreen_logo = "logo.png";
BMAP * splashscreen_background = "MenueBackground.png";

bool splashscreen_done;
bool splashscreen_begin_exit;
var splashscreen_startup;

void splashscreen_quit()
{
	if(splashscreen_begin_exit) return;
    splashscreen_begin_exit = true;
    splashscreen_startup = total_ticks;
}

void splashscreen_init()
{

}

void splashscreen_open()
{
    splashscreen_done = false;
    on_anykey = splashscreen_quit;
    splashscreen_startup = total_ticks;
    vec_set(screen_color, vector(1,1,1));
    splashscreen_begin_exit = false;
}

void splashscreen_update()
{
    var dt = total_ticks - splashscreen_startup;

    draw_quad(
        splashscreen_background,
        nullvector,
        NULL,
        NULL,
        vector(
            screen_size.x / bmap_width(splashscreen_background),
            screen_size.y / bmap_height(splashscreen_background),
            0
        ),
        NULL,
        310,
        0
    );

    var max_logo_scale = 0.75 * screen_size.y / bmap_height(splashscreen_logo);

    var logo_scale;
    var alpha = 100;
    if(splashscreen_begin_exit) {
        logo_scale = max_logo_scale + dt;
        alpha = maxv(0, 100 - 25 * dt);
        if(dt > 8) {
            splashscreen_done = true;
        }
    }
    else {
        if(dt < 32) {
            logo_scale = minv(max_logo_scale, powv(dt, 4) / 200000);
        } else {
            logo_scale = max_logo_scale;
        }
    }

    if(alpha > 0)
    {
        draw_quad(
            splashscreen_logo,
            vector(
                (screen_size.x - logo_scale * bmap_width(splashscreen_logo)) / 2,
                (screen_size.y - logo_scale * bmap_height(splashscreen_logo)) / 2,
                0
            ),
            NULL,
            NULL,
            vector(logo_scale, logo_scale, 0),
            NULL,
            alpha,
            0
        );
    }

}

void splashscreen_close()
{
    splashscreen_done = true;
    on_anykey = NULL;
}

bool splashscreen_isDone()
{
    return splashscreen_done;
}
