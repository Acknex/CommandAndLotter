#include <acknex.h>
#define PRAGMA_PATH "src"

#define wait _ich_bin_verboten_
#undef wait_for
#undef wait_for_my

#include "framework.h"

#define PRAGMA_PATH "ui"

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
#include "sputnik.c"
#include "enemy_hit.c"

