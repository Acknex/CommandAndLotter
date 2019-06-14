#include "game.h"
#include "map_loader.h"
#include "ui_game.h"
#include "framework.h"
#include <acknex.h>

void game_init(void)
{
	ui_game_init();
}

void game_open(void)
{
	framework_load_level(NULL);

	maploader_load("the-core.png");

	ui_game_open();
}

void game_update(void)
{
	ui_game_update();
}

void game_close(void)
{
	ui_game_close();
}

bool game_is_done(void)
{
	return key_esc;
}
