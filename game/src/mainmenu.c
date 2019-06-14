#include "mainmenu.h"
#include "music_player.h"

void mainmenu_init(void)
{

}

void mainmenu_open(void)
{
	// music_speedup(100);
	// music_start("media\\intro.mp3", 0.5, 0);
}

void mainmenu_update(void)
{
	draw_text(
		"1: start game\r\n2: credits\r\n3: exit",
		16,  16,
		COLOR_RED
	);
}

void mainmenu_close(void)
{

}

int mainmenu_get_response()
{
	if(key_1)
		return MAINMENU_RESPONSE_STARTGAME;
	if(key_2)
		return MAINMENU_RESPONSE_CREDIT;
	if(key_3)
		return MAINMENU_RESPONSE_EXIT;
	return MAINMENU_RESPONSE_KEEP;
}
