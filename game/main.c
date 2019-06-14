#define PRAGMA_PATH "src"

#define wait _ich_bin_verboten_
#undef wait_for
#undef wait_for_my

#include "src/framework.h"

#define PRAGMA_PATH "ui"
#define PRAGMA_PATH "map"

function main()
{
	framework_init();
}




// Include your code file here
#include "src/framework.c"
#include "src/game.c"
#include "src/mainmenu.c"
#include "src/credits.c"
#include "src/music_player.c"
#include "src/map_loader.c"

