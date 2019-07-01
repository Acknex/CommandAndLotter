#include "briefing.h"

BMAP * briefing_background   = "intro_background.png";
BMAP * briefing_startbtn     = "intro_start.png";
BMAP * briefing_rotor        = "intro_rotor.png";
BMAP * briefing_startbtn_dis = "intro_start_inactive.png";

SOUND * briefing_write_snd   = "intro_write.wav";

VECTOR briefing_startbutton_min, briefing_startbutton_max;

int briefing_result;
var briefing_startup;
bool briefing_ready;

char const * briefing_intro_text = "HELLO THIS IS A MISSION BRIEFING. I AM\nBRIEF. BUT I AM ALSO TOO LONG FOR THIS SHIT!\nFOR I AM A NEW LINE MESSAGE";

TEXT * briefing_render_txt =
{
    string("");
}

void briefing_mouseclick()
{
    if(!briefing_ready)
        return;
    if(mouse_pos.x < briefing_startbutton_min.x)
        return;
    if(mouse_pos.y < briefing_startbutton_min.y)
        return;
    if(mouse_pos.x >= briefing_startbutton_max.x)
        return;
    if(mouse_pos.y >= briefing_startbutton_max.y)
        return;
    briefing_result = BRIEFING_ACCEPT;
}

void briefing_abort()
{
    briefing_result = BRIEFING_ABORT;
}

void briefing_init()
{

}

void briefing_open()
{
    on_mouse_left = briefing_mouseclick;
    on_esc = briefing_abort;
    briefing_result = BRIEFING_PENDING;
    briefing_startup = total_ticks;
    briefing_ready = false;
}

void briefing_update()
{
    var dt = total_ticks - briefing_startup;

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

    var intro_speed = 1; // 16 chars per frame

    int char_cnt = dt / intro_speed;
    if(char_cnt > 0)
    {
        // render intro text
        if(char_cnt <= strlen(briefing_intro_text))
        {
            str_printf((briefing_render_txt->pstring)[0], "%.*s", char_cnt, briefing_intro_text);
            if(char_cnt != (int)((dt - time_frame) / intro_speed))
            {
                snd_play(briefing_write_snd, 100, 0);
            }
        }
        else
        {
            briefing_ready = true;
        }

        briefing_render_txt->scale_x = 4 * scale;
        briefing_render_txt->scale_y = 4 * scale;
        briefing_render_txt->pos_x = pos_x + scale * 137;
        briefing_render_txt->pos_y = pos_y + scale * 190;
        briefing_render_txt->size_x = 998 / 4;
        briefing_render_txt->size_y = 768 / 4;
        briefing_render_txt->flags = WWRAP;

        draw_obj(briefing_render_txt);
    }

    if(briefing_ready)
    {
        // render button
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
    else
    {
        // render "loading circle"
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
    }
}

void briefing_close()
{
    on_mouse_left = NULL;
    on_esc = NULL;
}

int briefing_getResult()
{
    return briefing_result;
}
