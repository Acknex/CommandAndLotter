#include "game.h"
#include "map_loader.h"
#include "ui_game.h"
#include "framework.h"
#include "camera.h"
#include "sputnik.h"
#include "stub.h"
<<<<<<< .mine
#include "stub.h"
=======

>>>>>>> .theirs
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
	topdown_camera_open();
	SPUTNIK_Init();
	stub_init(); //hook debug shit here
}

void game_update(void)
{
	ui_game_update();
<<<<<<< .mine
	SPUTNIK_Update();
	stub_update(); //hook debug shit here
=======
	topdown_camera_update();

>>>>>>> .theirs
}

void game_close(void)
{
	ui_game_close();
}

bool game_is_done(void)
{
	return key_esc;
}
