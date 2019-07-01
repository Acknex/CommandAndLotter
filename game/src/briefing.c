#include "briefing.h"

BMAP * briefing_background   = "intro_background.png";
BMAP * briefing_startbtn     = "intro_start.png";
BMAP * briefing_rotor        = "intro_rotor.png";
BMAP * briefing_startbtn_dis = "intro_start_inactive.png";

VECTOR briefing_startbutton_min, briefing_startbutton_max;

bool briefing_done;

void briefing_mouseclick()
{
    if(mouse_pos.x < briefing_startbutton_min.x)
        return;
    if(mouse_pos.y < briefing_startbutton_min.y)
        return;
    if(mouse_pos.x >= briefing_startbutton_max.x)
        return;
    if(mouse_pos.y >= briefing_startbutton_max.y)
        return;
    briefing_done = true;
}

void briefing_init()
{

}

void briefing_open()
{
    on_mouse_left = briefing_mouseclick;
    briefing_done = false;
}

void briefing_update()
{
    var scale_x = screen_size.x / bmap_width(briefing_background);
    var scale_y = screen_size.y / bmap_height(briefing_background);
    var scale = minv(scale_x, scale_y);

    var pos_x = (screen_size.x - scale * bmap_width(briefing_background)) / 2;
    var pos_y = (screen_size.y - scale * bmap_height(briefing_background)) / 2;

    draw_quad(
        briefing_background,
        vector(pos_x, pos_y, 0),
        NULL,
        NULL,
        vector(scale, scale, 0),
        NULL,
        100,
        0);

    draw_quad(
        briefing_rotor,
        vector(pos_x + scale * 1688, pos_y + scale * 764, 0),
        NULL,
        NULL,
        vector(scale, scale, 0),
        NULL,
        100,
        -total_ticks
    );

    briefing_startbutton_min.x = pos_x + scale * 1613;
    briefing_startbutton_min.y = pos_y + scale * 59;
    briefing_startbutton_max.x = briefing_startbutton_min.x + scale * bmap_width(briefing_startbtn);
    briefing_startbutton_max.y = briefing_startbutton_min.y + scale * bmap_height(briefing_startbtn);

    BMAP * bmp = briefing_startbtn_dis;
    if(mouse_pos.x >= briefing_startbutton_min.x && mouse_pos.y >= briefing_startbutton_min.y)
    {
        if(mouse_pos.x < briefing_startbutton_max.x && mouse_pos.y < briefing_startbutton_max.y)
        {
            bmp = briefing_startbtn;
        }
    }

    draw_quad(
        bmp,
        briefing_startbutton_min,
        NULL,
        NULL,
        vector(scale, scale, 0),
        NULL,
        100,
        0
    );
}

void briefing_close()
{
    on_mouse_left = NULL;
}

bool briefing_isDone()
{
    return briefing_done;
}
