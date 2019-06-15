#include "mainmenu.h"
#include "music_player.h"


void mainmenu_init(void)
{
	wndMenuMain = uimenu_window_create_borderless(50, 50, 128, 32 * 5);
	uimenu_element_t * btnNewGame = uimenu_make_button(0, 32 * 0, 128, 32, mainmenu_btn_new_game, mainmenu_btn_new_game_hi, mainmenu_btn_new_game_hi, mainmenu_set_start);
	uimenu_element_t * btnOptions = uimenu_make_button(0, 32 * 1, 128, 32, mainmenu_btn_options, mainmenu_btn_options_hi, mainmenu_btn_options_hi, NULL);
	uimenu_element_t * btnCredits = uimenu_make_button(0, 32 * 2, 128, 32, mainmenu_btn_credits, mainmenu_btn_credits_hi, mainmenu_btn_credits_hi, mainmenu_set_credits);
	uimenu_element_t * btnExit = uimenu_make_button(0, 32 * 4, 128, 32, mainmenu_btn_exit, mainmenu_btn_exit_hi, mainmenu_btn_exit_hi, mainmenu_set_exit);

	uimenu_add_element_to_window(wndMenuMain, btnNewGame);
	uimenu_add_element_to_window(wndMenuMain, btnOptions);
	uimenu_add_element_to_window(wndMenuMain, btnCredits);
	uimenu_add_element_to_window(wndMenuMain, btnExit);

	uimenu_window_initialize(wndMenuMain);
	uimenu_window_show(wndMenuMain);
	
	// mainMenuWindow2 = uimenu_window_create(150, 50, 128, 32 * 5, "Menue");
	// uimenu_element_t * btnNewGame2 = uimenu_make_button(0, 32 * 0, 128, 32, mainmenu_btn_new_game, mainmenu_btn_new_game_hi, mainmenu_btn_new_game_hi, mainmenu_set_start);
	// uimenu_element_t * btnOptions2 = uimenu_make_button(0, 32 * 1, 128, 32, mainmenu_btn_options, mainmenu_btn_options_hi, mainmenu_btn_options_hi, NULL);
	// uimenu_element_t * btnCredits2 = uimenu_make_button(0, 32 * 2, 128, 32, mainmenu_btn_credits, mainmenu_btn_credits_hi, mainmenu_btn_credits_hi, mainmenu_set_credits);
	// uimenu_element_t * btnExit2 = uimenu_make_button(0, 32 * 4, 128, 32, mainmenu_btn_exit, mainmenu_btn_exit_hi, mainmenu_btn_exit_hi, mainmenu_set_exit);

	// uimenu_add_element_to_window(mainMenuWindow2, btnNewGame2);
	// uimenu_add_element_to_window(mainMenuWindow2, btnOptions2);
	// uimenu_add_element_to_window(mainMenuWindow2, btnCredits2);
	// uimenu_add_element_to_window(mainMenuWindow2, btnExit2);

	// uimenu_window_initialize(mainMenuWindow2);
	// uimenu_window_show(mainMenuWindow2);
}


void mainmenu_set_start(void)
{
	mainmenu_current_response = MAINMENU_RESPONSE_STARTGAME;
}
void mainmenu_set_credits(void)
{
	mainmenu_current_response = MAINMENU_RESPONSE_CREDIT;
}
void mainmenu_set_exit(void)
{
	mainmenu_current_response = MAINMENU_RESPONSE_EXIT;
}

void mainmenu_open(void)
{
	music_speedup(100);
	music_start("media/mainmenu.mp3", 0.5, 0);
}

void mainmenu_update(void)
{
	// draw_text(
	// 	"1: start game\r\n2: credits\r\n3: exit",
	// 	16,  16,
	// 	COLOR_RED
	// );
}

void mainmenu_close(void)
{
	uimenu_window_destroy(wndMenuMain);
}

int mainmenu_get_response()
{
	return mainmenu_current_response;
	// if(key_1)
	// 	return MAINMENU_RESPONSE_STARTGAME;
	// if(key_2)
	// 	return MAINMENU_RESPONSE_CREDIT;
	// if(key_3)
	// 	return MAINMENU_RESPONSE_EXIT;
	// return MAINMENU_RESPONSE_KEEP;
}
