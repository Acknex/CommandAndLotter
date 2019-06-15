#include "mainmenu.h"
#include "music_player.h"


void mainmenu_init(void)
{
	mainmenu_wndMenuBackground = uimenu_window_create_borderless(0, 0, 1920, 1080);
	mainmenu_wndMenuBackground->_forced_background = mainmenu_background;

	mainmenu_wndMenuMain = uimenu_window_create_borderless(0, 0, 1920, 1080);

	int btnHeight = bmap_height(mainmenu_btn_new_game);
	mainmenu_btnNewGame = uimenu_make_button( (1920 / 2) - (bmap_width(mainmenu_btn_new_game) / 2), 300 + (btnHeight * 0), 128, 32, mainmenu_btn_new_game, mainmenu_btn_new_game, mainmenu_btn_new_game, mainmenu_set_start);
	mainmenu_btnOptions = uimenu_make_button( (1920 / 2) - (bmap_width(mainmenu_btn_options) / 2), 300 + (btnHeight * 1), 128, 32, mainmenu_btn_options, mainmenu_btn_options, mainmenu_btn_options, NULL);
	mainmenu_btnCredits = uimenu_make_button( (1920 / 2) - (bmap_width(mainmenu_btn_credits) / 2), 300 + (btnHeight * 2), 128, 32, mainmenu_btn_credits, mainmenu_btn_credits, mainmenu_btn_credits, mainmenu_set_credits);
	mainmenu_btnExit = uimenu_make_button( (1920 / 2) - (bmap_width(mainmenu_btn_exit) / 2), 300 + (btnHeight * 4), 128, 32, mainmenu_btn_exit, mainmenu_btn_exit, mainmenu_btn_exit, mainmenu_set_exit);

	uimenu_add_element_to_window(mainmenu_wndMenuMain, mainmenu_btnNewGame);
	uimenu_add_element_to_window(mainmenu_wndMenuMain, mainmenu_btnOptions);
	uimenu_add_element_to_window(mainmenu_wndMenuMain, mainmenu_btnCredits);
	uimenu_add_element_to_window(mainmenu_wndMenuMain, mainmenu_btnExit);

	uimenu_window_initialize(mainmenu_wndMenuMain);
	uimenu_window_initialize(mainmenu_wndMenuBackground);

	mainmenu_wndMenuMain->_content_panel->scale_x = mainmenu_wndMenuBackground->_content_panel->scale_x = screen_size.x  / 1920;
	mainmenu_wndMenuMain->_content_panel->scale_y = mainmenu_wndMenuBackground->_content_panel->scale_y = screen_size.y  / 1080;

	uimenu_window_show(mainmenu_wndMenuMain);
	uimenu_window_show(mainmenu_wndMenuBackground);
	
	// TESTING
	// uimenu_window_t * mainMenuWindow2 = uimenu_window_create(150, 50, 128, 32 * 5, "Menue");
	// uimenu_element_t * btnNewGame2 = uimenu_make_button(0, 32 * 0, 128, 32, mainmenu_btn_new_game, mainmenu_btn_new_game_hi, mainmenu_btn_new_game_hi, mainmenu_set_start);
	// uimenu_element_t * btnOptions2 = uimenu_make_button(0, 32 * 1, 128, 32, mainmenu_btn_options, mainmenu_btn_options_hi, mainmenu_btn_options_hi, NULL);
	// uimenu_element_t * btnCredits2 = uimenu_make_button(0, 32 * 2, 128, 32, mainmenu_btn_credits, mainmenu_btn_credits_hi, mainmenu_btn_credits_hi, mainmenu_set_credits);
	// uimenu_element_t * btnExit2 = uimenu_make_simple_button(0, 32 * 4, 128, 32, "exit", uimenu_default_font, mainmenu_set_exit);

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

void mainmenu_show_options(void)
{
	
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
	if(mainmenu_wndMenuMain != NULL)
	{
		if(total_secs % 2)
			mainmenu_wndMenuMain->_panel->alpha = 25 + random(75);
	}
}

void mainmenu_resolution_update(void)
{
	if(mainmenu_wndMenuMain != NULL)
	{
		mainmenu_wndMenuMain->_content_panel->scale_x = mainmenu_wndMenuBackground->_content_panel->scale_x = screen_size.x  / 1920;
		mainmenu_wndMenuMain->_content_panel->scale_y = mainmenu_wndMenuBackground->_content_panel->scale_y = screen_size.y  / 1080;
	}
}

void mainmenu_close(void)
{
	uimenu_window_destroy(mainmenu_wndMenuBackground);
	uimenu_window_destroy(mainmenu_wndMenuMain);
}

int mainmenu_get_response()
{
	return mainmenu_current_response;
}
