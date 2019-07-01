#include "briefing.h"

typedef struct BRIEFING_NODE_tag
{
    char const * text; // what text to print
    int faceset; // which face to show
    var post_delay; // delay after in "text characters"
    var pre_delay; // delay before in "text characters"
    struct BRIEFING_NODE_tag * next; // what is next (or NULL if end)
} BRIEFING_NODE;


BMAP * briefing_background   = "intro_background.png";
BMAP * briefing_startbtn     = "intro_start.png";
BMAP * briefing_rotor        = "intro_rotor.png";
BMAP * briefing_startbtn_dis = "intro_start_inactive.png";

SOUND * briefing_write_snd   = "intro_write.wav";

FONT * briefing_font = "Dot Matrix#32";

TEXT * briefing_render_txt =
{
    string("");
    font = briefing_font;
}

#define BRIEFING_FACESET_SPUTNIK 0
#define BRIEFING_FACESET_CBABE   1
#define BRIEFING_FACESET_EYE     2
#define BRIEFING_FACESET_ESEL    3
#define BRIEFING_FACESET_SKULL   4

// sorry, but this is bottom to top.

BRIEFING_NODE * briefing_txt_03 = {
    text = "Roaaaar! Eselslerche meldet sich bereit zum Einsatz!";
    faceset = BRIEFING_FACESET_ESEL;
    next = NULL;
    pre_delay = 4;
    post_delay = 4;
}

BRIEFING_NODE * briefing_txt_02 = {
    text = "Und ich bin das heisse C-Babe!";
    faceset = BRIEFING_FACESET_CBABE;
    next = briefing_txt_03;
    pre_delay = 4;
}

BRIEFING_NODE * briefing_txt_01 = {
    text = "Hello, World! Ich bin der Skull!";
    faceset = BRIEFING_FACESET_SKULL;
    next = briefing_txt_02;
    pre_delay = 8; // delay in ticks
}

BMAP * briefing_faceset_sputnik[3];
BMAP * briefing_faceset_cbabe[3];
BMAP * briefing_faceset_eye[3];
BMAP * briefing_faceset_esel[3];
BMAP * briefing_faceset_skull[3];

BMAP ** briefing_facesets[5];

BMAP * briefing_faceset_sputnik_0 = "Sputnik1.png";
BMAP * briefing_faceset_sputnik_1 = "Sputnik2.png";
BMAP * briefing_faceset_sputnik_2 = "Sputnik3.png";

BMAP * briefing_faceset_cbabe_0 = "CbFace1.png";
BMAP * briefing_faceset_cbabe_1 = "CbFace2.png";
BMAP * briefing_faceset_cbabe_2 = "CbFace3.png";

BMAP * briefing_faceset_eye_0 = "IrisSet2.png";
BMAP * briefing_faceset_eye_1 = "IrisSet3.png";
BMAP * briefing_faceset_eye_2 = "IrisSet4.png";

BMAP * briefing_faceset_esel_0 = "ESFace1.png";
BMAP * briefing_faceset_esel_1 = "ESFace2.png";
BMAP * briefing_faceset_esel_2 = "ESFace3.png";

BMAP * briefing_faceset_skull_0 = "WkFace4.png";
BMAP * briefing_faceset_skull_1 = "WkFace5.png";
BMAP * briefing_faceset_skull_2 = "WkFace6.png";


int briefing_text_cursor;
BRIEFING_NODE * briefing_current_node;
var briefing_countdown;
int briefing_current_mode;

VECTOR briefing_startbutton_min, briefing_startbutton_max;

int briefing_result;
var briefing_startup;
bool briefing_ready;

void briefing_init()
{
    AddFontResource("media/dotmatrix.ttf");

    briefing_facesets[BRIEFING_FACESET_SPUTNIK] = briefing_faceset_sputnik;
    briefing_facesets[BRIEFING_FACESET_CBABE]   = briefing_faceset_cbabe;
    briefing_facesets[BRIEFING_FACESET_EYE]     = briefing_faceset_eye;
    briefing_facesets[BRIEFING_FACESET_ESEL]    = briefing_faceset_esel;
    briefing_facesets[BRIEFING_FACESET_SKULL]   = briefing_faceset_skull;

    briefing_faceset_sputnik[0] = briefing_faceset_sputnik_0;
    briefing_faceset_sputnik[1] = briefing_faceset_sputnik_1;
    briefing_faceset_sputnik[2] = briefing_faceset_sputnik_2;

    briefing_faceset_cbabe[0] = briefing_faceset_cbabe_0;
    briefing_faceset_cbabe[1] = briefing_faceset_cbabe_1;
    briefing_faceset_cbabe[2] = briefing_faceset_cbabe_2;

    briefing_faceset_eye[0] = briefing_faceset_eye_0;
    briefing_faceset_eye[1] = briefing_faceset_eye_1;
    briefing_faceset_eye[2] = briefing_faceset_eye_2;

    briefing_faceset_esel[0] = briefing_faceset_esel_0;
    briefing_faceset_esel[1] = briefing_faceset_esel_1;
    briefing_faceset_esel[2] = briefing_faceset_esel_2;

    briefing_faceset_skull[0] = briefing_faceset_skull_0;
    briefing_faceset_skull[1] = briefing_faceset_skull_1;
    briefing_faceset_skull[2] = briefing_faceset_skull_2;
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

void briefing_open()
{
    on_mouse_left = briefing_mouseclick;
    on_esc = briefing_abort;
    briefing_result = BRIEFING_PENDING;
    briefing_startup = total_ticks;
    briefing_ready = false;

    briefing_current_node = briefing_txt_01;
    briefing_text_cursor = 0;
    briefing_current_mode = 0;

    str_cpy((briefing_render_txt->pstring)[0], "");
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

    // render intro text
    if((int)(dt / intro_speed) != (int)((dt - time_frame) / intro_speed))
    {
        if(briefing_current_node != NULL)
        {
            // no switch as we change the mode on the go and we
            // want next-state evaluation!

            // state: init
            if(briefing_current_mode == 0)
            {
                if(briefing_current_node->pre_delay > 0) {
                    briefing_countdown = briefing_current_node->pre_delay;
                    briefing_current_mode = 1;
                } else {
                    briefing_current_mode = 2;
                }
            }

            // state: pre-delay
            if(briefing_current_mode == 1)
            {
                if(briefing_countdown <= 0) {
                    briefing_current_mode = 2;
                }
                briefing_countdown -= 1;
            }

            // state: render text
            if(briefing_current_mode == 2)
            {
                char str[2];
                str[0] = (briefing_current_node->text)[briefing_text_cursor];
                str[1] = 0;

                if(str[0] != '\0')
                {
                    str_cat((briefing_render_txt->pstring)[0], str);
                    briefing_text_cursor += 1;
                }
                else
                {
                    str_cat((briefing_render_txt->pstring)[0], "\n");
                    briefing_text_cursor = 0;

                    if(briefing_current_node->post_delay > 0)
                    {
                        briefing_countdown = briefing_current_node->post_delay;
                        briefing_current_mode = 3;
                    }
                    else
                    {
                        briefing_current_mode = 4;
                    }

                }

                snd_play(briefing_write_snd, 100, 0);
            }

            // state: post-delay
            if(briefing_current_mode == 3)
            {
                if(briefing_countdown <= 0) {
                    briefing_current_mode = 4;
                }
                briefing_countdown -= 1;
            }

            // state: completed
            if(briefing_current_mode == 4)
            {
                briefing_current_node = briefing_current_node->next;
                briefing_current_mode = 0;
            }
        }
        else
        {
            briefing_ready = true;
        }
    }

    // render face set only when node is active and we are in text mode
    if(briefing_current_node != NULL && briefing_current_mode == 2)
    {
        int index = briefing_current_node->faceset;
        BMAP ** faceset = briefing_facesets[index];
        BMAP * bmp = (faceset[(total_ticks / 4) % 3]);
        draw_quad(
            bmp,
            vector(scale * 1266, scale * 179, 0),
            NULL,
            NULL,
            vector(scale * 192 / bmap_width(bmp), scale * 186 / bmap_height(bmp), 0),
            NULL,
            100,
            0
        );
    }

    // briefing_render_txt->scale_x = 4 * scale;
    // briefing_render_txt->scale_y = 4 * scale;
    briefing_render_txt->pos_x = pos_x + scale * 137;
    briefing_render_txt->pos_y = pos_y + scale * 190;
    briefing_render_txt->size_x = scale * 998;
    briefing_render_txt->size_y = scale * 768;
    briefing_render_txt->flags = WWRAP;

    draw_obj(briefing_render_txt);

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
