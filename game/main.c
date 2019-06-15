#include <acknex.h>
#define PRAGMA_PATH "src"
#define PRAGMA_PATH "code"

#define PRAGMA_PATH "%EXE_DIR%\\code"; // fx files

#define DEBUG

#ifdef DEBUG
#include <default.c>
#endif

#define wait _ich_bin_verboten_
#undef wait_for
#undef wait_for_my

#include "framework.h"

#define PRAGMA_PATH "ui"
#define PRAGMA_PATH "map"

BMAP * shader_noise = "noise.png";

function main()
{
	framework_init();
}




// Include your code file here
#include "framework.c"
#include "game.c"
#include "mainmenu.c"
#include "credits.c"
#include "music_player.c"
#include "ui_game.c"
#include "map_loader.c"
#include "sputnik.c"
#include "enemy_hit.c"
#include "camera.c"
#include "UnitMangement.c"
#include "stub.c"
#include "unit.c"
#include "ang.c"
#include "scan.c"

