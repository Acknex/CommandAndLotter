#include "game.h"
#include "map_loader.h"
#include "ui_game.h"
#include "framework.h"
#include "grid.h"
#include "camera.h"
#include "sputnik.h"
#include "spawner.h"
#include "stub.h"
#include <acknex.h>
#include "UnitMangement.h"
#include "building.h"
#include "fow.h"
#include "z.h"
#include "materials.h"
#include "effects2d.h"

void game_init(void)
{
	ui_game_init();
    UnitMangement_init();
    grid_init();
	buildingPlacement_init();
	presetsInit();
    effects2d_init();

    effect_load(mtl_model, "units.fx");
}

void game_open(void)
{
	#ifdef GAME_OPEN_DEBUG
		cprintf1("\n game_open: START at frame %d, calling framework_load_level() now...", (int)total_frames);
	#endif
	framework_load_level(NULL);

    sun_light = 100;

	#ifdef GAME_OPEN_DEBUG
		cprintf0("\n - calling jpsMapLoadFromFile() now...");
	#endif
	int loadResult = (jpsMapLoadFromFile("map0.txt") != NULL);
	#ifdef GAME_OPEN_DEBUG
		cprintf1(" result: %d", loadResult);
		cprintf0("\n - calling maploader_load() now...");
	#endif
	maploader_load("the-core.png");

	#ifdef GAME_OPEN_DEBUG
		cprintf0("\n - calling ui_game_open() now...");
	#endif
	ui_game_open();
	topdown_camera_open();

    UnitMangement_open();
    fow_open();

    //SetupPostprocessing();

	SPUTNIK_Init();
	SPAWNER_Init();
	Z_Init();
	stub_init(); //hook debug shit here

	grid_open(50,50);
	buildingPlacement_open();
	#ifdef GAME_OPEN_DEBUG
		cprintf1("\n game_open: END at frame %d", (int)total_frames);
	#endif

    effects2d_open();
}

void game_update(void)
{
    //updateRenderTargetsIfNeeded();
    topdown_camera_update();
    ui_game_update();
	jpsGameUpdate(mapGetCurrent());
	SPUTNIK_Update();
	SPAWNER_Update();
	Z_Update();
	stub_update(); //hook debug shit here
    UnitMangement_update();
    fow_update();
	buildingPlacement_update();

    effects2d_update();
}

void game_close(void)
{
	ui_game_close();
	grid_close();
    effects2d_close();
}

bool game_is_done(void)
{
	return key_esc;
}
