#include "game.h"
#include "map_loader.h"
#include "ui_game.h"
#include "framework.h"
#include "camera.h"
#include <acknex.h>
#include "UnitMangement.h"

void game_init(void)
{
	ui_game_init();
    UnitMangement_init();
}

void game_open(void)
{
	framework_load_level(NULL);

	maploader_load("the-core.png");

	ui_game_open();
	topdown_camera_open();
    UnitMangement_open();
}

void game_update(void)
{
	ui_game_update();
	topdown_camera_update();
    UnitMangement_update();
}

void game_close(void)
{
	ui_game_close();
}

bool game_is_done(void)
{
	return key_esc;
}
