#define PRAGMA_PATH "src"


#define wait _ich_bin_verboten_
#undef wait_for
#undef wait_for_my

#include "framework.h"

#define PRAGMA_PATH "ui"
#define PRAGMA_PATH "map"

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
#include "UnitMangement.c"
