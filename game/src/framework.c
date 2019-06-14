#include "framework.h"
#include "global.h"
#include "mainmenu.h"
#include "game.h"
#include "credits.h"
#include "music_player.h"

#include <acknex.h>
#include <windows.h>

#define FRAMEWORK_STATE_SHUTDOWN    -1
#define FRAMEWORK_STATE_STARTUP      0
#define FRAMEWORK_STATE_MAINMENU     1
#define FRAMEWORK_STATE_GAME         2
#define FRAMEWORK_STATE_CREDITS      3

typedef struct
{
    int state;
    int nextState;
    int frameCounter;
    int loaderState;
} framework_t;

framework_t framework;

// BMAP * framework_mouse_cursor = "cursor3.tga";

// BMAP * framework_load_screen_bmap = "game_loadingscreen.png";

//PANEL * framework_load_screen =
//{
//    bmap = framework_load_screen_bmap;
//    size_x = 1280;
//    size_y = 720;
//    flags = TRANSLUCENT;
//    layer = 900;
//}

void framework_update_settings()
{
//	fps_max = settings.fps_limit;
//	d3d_anisotropy = settings.anisotropy;
//	d3d_triplebuffer = settings.vsync;
//	video_gamma = settings.gamma;

//	video_set(
//			settings.resolution_x,
//			settings.resolution_y,
//			0,
//			2 - !!settings.fullscreen); // !! -> 0 oder 1

	// Ladebildschirm passend skalieren
	// framework_load_screen.scale_x = screen_size.x / framework_load_screen.size_x;
	// framework_load_screen.scale_y = screen_size.y / framework_load_screen.size_y;
}

//! Initialisiert das Spiel und so
void framework_init()
{
    // settings_init();
    // settings_load();

    fps_min = 25; // overshoot vermeiden, v.a. wenn's ruckelt

    framework_update_settings();
    // settings_register_signal(framework_update_settings);

    particle_mode = 8;
    collision_mode = 2;
    preload_mode = 3; // preload a lot

    // vec_set(sky_color, vector(1,1,1));

    // mouse_map = framework_mouse_cursor;

#ifndef FRAMEWORK_NO_POSTPROCESS
    // SetupDefaultMaterials();
    // SetupPostprocessing();
#endif

    on_frame = framework_update;
}

void framework_setup(ENTITY * ent, int subsystem)
{
    if(ent == NULL) error("framework: framework_setup mit NULL aufgerufen!");
    ent->SK_SUBSYSTEM = subsystem;
}

void framework_freeze(ENTITY * ent)
{
    if(ent == NULL) error("framework: framework_freeze mit NULL aufgerufen!");
    ent->SK_SUBSYSTEM = 0;
    ent->emask &= ~DYNAMIC;
}

void framework_remove(ENTITY * ent)
{
	if(ent == NULL) error("framework: framework_freeze mit NULL aufgerufen!");
	ent->SK_ENTITY_DEAD = 1;
}

void framework_transfer(int state)
{
    framework.nextState = state;
}

void framework_cleanup()
{
    ENTITY * ent;
    ent = ent_next(NULL);
    while(ent)
    {
        you = ent;
        ent = ent_next(ent);
        if(you->SK_ENTITY_DEAD)
            ptr_remove(you);
    }
}

void framework_capture_mouse()
{
    RECT rect;
    GetWindowRect(hWnd, &rect);
    SetCursorPos((rect.left + rect.right) / 2, (rect.top + rect.bottom) / 2);
}

//! Aktualisiert alles.
void framework_update()
{
#ifdef DEBUG
    if(key_alt && key_f4)
        framework_transfer(FRAMEWORK_STATE_SHUTDOWN);
#endif

#ifdef DEBUG_NO_ESCAPE_QUIT
    on_esc = NULL;
#endif

    switch(framework.state)
    {
    case FRAMEWORK_STATE_STARTUP:
        if(framework.frameCounter == 1)
        {
            // spiel im ersten frame initialisieren
						music_init();
						mainmenu_init();
						game_init();
						credits_init();
#ifdef DEBUG_FRAMEWORK_FASTSTART
            if(settings.skipIntro)
                framework_transfer(FRAMEWORK_STATE_LOAD);
            else
                framework_transfer(FRAMEWORK_STATE_INTRO);
#else
            framework_transfer(FRAMEWORK_STATE_MAINMENU);
#endif
        }
        break;

    case FRAMEWORK_STATE_MAINMENU:
    {
        mainmenu_update();
        int response = mainmenu_get_response();
        if(response != MAINMENU_RESPONSE_KEEP)
        {
            switch(response)
            {
            case MAINMENU_RESPONSE_STARTGAME:
								framework_transfer(FRAMEWORK_STATE_GAME);
                break;
            case MAINMENU_RESPONSE_CREDIT:
                framework_transfer(FRAMEWORK_STATE_CREDITS);
                break;
            case MAINMENU_RESPONSE_EXIT:
                framework_transfer(FRAMEWORK_STATE_SHUTDOWN);
                break;
            default:
                error("framework: unknown response from main menu.");
            }
        }
        break;
    }

    case FRAMEWORK_STATE_CREDITS:
        credits_update();
        if(credits_isdone())
            framework_transfer(FRAMEWORK_STATE_MAINMENU);
        break;

    case FRAMEWORK_STATE_GAME:
        game_update();
        if(game_is_done())
					framework_transfer(FRAMEWORK_STATE_MAINMENU);
        break;

    default:
        error(str_printf(NULL, "framework: unsupported state %d!", framework.state));

    }

    if(framework.state != framework.nextState)
    {
        diag(str_printf(NULL, "\nstate transition from %d to %d.", framework.state, framework.nextState));

        switch(framework.state)
        {
        case FRAMEWORK_STATE_SHUTDOWN:
            error("framework: shutdown state should never be left again!");
            break;

        case FRAMEWORK_STATE_STARTUP:
            break;

        case FRAMEWORK_STATE_MAINMENU:
            mainmenu_close();
            break;

        case FRAMEWORK_STATE_CREDITS:
            credits_close();
            break;

        case FRAMEWORK_STATE_GAME:
            game_close();
            break;

        default:
            error(str_printf(NULL, "framework: unsupported state %d!", framework.state));
        }

        framework.state = framework.nextState;

        switch(framework.state)
        {
        case FRAMEWORK_STATE_SHUTDOWN:
            break;

        case FRAMEWORK_STATE_STARTUP:
            error("framework: startup state should never be entered again!");
            break;

        case FRAMEWORK_STATE_MAINMENU:
						mainmenu_open();
            break;

        case FRAMEWORK_STATE_CREDITS:
            credits_open();
            break;

        case FRAMEWORK_STATE_GAME:
            // game was already openend
            // by LOAD stateg
						game_open();
            break;

        default:
            error(str_printf(NULL, "framework: unsupported state %d!", framework.state));
        }
    }

    // Cleanup all dead entities
    framework_cleanup();

    if(framework.state == FRAMEWORK_STATE_SHUTDOWN)
    {
        on_frame = NULL;
        sys_exit(NULL);
    }

    framework.frameCounter += 1;
}

