#include "game.h"
#include "map_loader.h"
#include "ui_game.h"
#include "framework.h"
#include "grid.h"
#include "camera.h"
#include "sputnik.h"
#include "stub.h"
#include <acknex.h>
#include "UnitMangement.h"
#include "building.h"

void game_init(void)
{
	ui_game_init();
    UnitMangement_init();
    grid_init();
	buildingPlacement_init();
}

void game_open(void)
{
	framework_load_level(NULL);

    sun_light = 100;

	maploader_load("the-core.png");

	ui_game_open();
	topdown_camera_open();

    UnitMangement_open();
	SPUTNIK_Init();
	stub_init(); //hook debug shit here
	
	grid_open(50,50);
	buildingPlacement_open();
}

void game_update(void)
{
	ui_game_update();
	SPUTNIK_Update();
	stub_update(); //hook debug shit here
	topdown_camera_update();
    UnitMangement_update();
	buildingPlacement_update();
}

void game_close(void)
{
	ui_game_close();
	grid_close();
}

bool game_is_done(void)
{
	return key_esc;
}
