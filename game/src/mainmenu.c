#include "mainmenu.h"
#include "music_player.h"


void mainmenu_init(void)
{
	// Main Menu
	mainmenu_wndMenuBackground = uimenu_window_create_borderless(0, 0, 1920, 1080);
	mainmenu_wndMenuBackground->_forced_background = mainmenu_background;

	mainmenu_wndMenuMain = uimenu_window_create_borderless(0, 0, 1920, 1080);

	int btnHeight = bmap_height(mainmenu_btn_new_game);
	mainmenu_btnNewGame = uimenu_make_button( (1920 / 2) - (bmap_width(mainmenu_btn_new_game) / 2), 300 + (btnHeight * 0), 128, 32, mainmenu_btn_new_game, mainmenu_btn_new_game, mainmenu_btn_new_game, mainmenu_set_start);
	mainmenu_btnOptions = uimenu_make_button( (1920 / 2) - (bmap_width(mainmenu_btn_options) / 2), 300 + (btnHeight * 1), 128, 32, mainmenu_btn_options, mainmenu_btn_options, mainmenu_btn_options, mainmenu_show_options);
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


	// Options Menu
	mainmenu_wndMenuOptions = uimenu_window_create( (screen_size.x / 2) - (640 / 2), (screen_size.y / 2) - (480 / 2), 640, 480, "Options");
	uimenu_element_t * optionsExit = uimenu_make_simple_button(640 - 85, 480 - 28, 85, 32, "OK", uimenu_default_font, mainmenu_hide_options);

	uimenu_element_t * optionsTabGeneral = uimenu_make_simple_button(0, 0, 32, "General", uimenu_default_font, mainmenu_show_options_general);
	uimenu_element_t * optionsTabVideo = uimenu_make_simple_button(optionsTabGeneral->x + optionsTabGeneral->width + 2, 0, 32, "Video", uimenu_default_font, mainmenu_show_options_video);
	uimenu_element_t * optionsTabAudio = uimenu_make_simple_button(optionsTabVideo->x + optionsTabVideo->width + 2, 0, 32, "Audio", uimenu_default_font, mainmenu_show_options_audio);
	uimenu_element_t * optionsTabControls = uimenu_make_simple_button(optionsTabAudio->x + optionsTabAudio->width + 2, 0, 32, "Controls", uimenu_default_font, mainmenu_show_options_controls);

	uimenu_add_element_to_window(mainmenu_wndMenuOptions, optionsExit);

	uimenu_add_element_to_window(mainmenu_wndMenuOptions, optionsTabGeneral);
	uimenu_add_element_to_window(mainmenu_wndMenuOptions, optionsTabVideo);
	uimenu_add_element_to_window(mainmenu_wndMenuOptions, optionsTabAudio);
	uimenu_add_element_to_window(mainmenu_wndMenuOptions, optionsTabControls);

	uimenu_window_initialize(mainmenu_wndMenuOptions);

	// General TAB
	mainmenu_wndMenuOptionsGeneral = uimenu_window_create_borderless(0, 32, 640 - 8, 380);

	uimenu_element_t * generalInfo = uimenu_make_text(5, 5, 100, 32, "Nothing to see here", NULL, NULL);

	uimenu_add_element_to_window(mainmenu_wndMenuOptionsGeneral, generalInfo);

	uimenu_slave_window_to_window(mainmenu_wndMenuOptions, mainmenu_wndMenuOptionsGeneral);

	uimenu_window_initialize(mainmenu_wndMenuOptionsGeneral);
}


void mainmenu_set_start(void)
{
	mainmenu_current_response = MAINMENU_RESPONSE_STARTGAME;
}

void mainmenu_show_options_general(void)
{
	uimenu_window_show(mainmenu_wndMenuOptionsGeneral);
	uimenu_window_hide(mainmenu_wndMenuOptionsVideo);
	uimenu_window_hide(mainmenu_wndMenuOptionsAudio);
	uimenu_window_hide(mainmenu_wndMenuOptionsControls);
}

void mainmenu_show_options_video(void)
{
	uimenu_window_hide(mainmenu_wndMenuOptionsGeneral);
	uimenu_window_show(mainmenu_wndMenuOptionsVideo);
	uimenu_window_hide(mainmenu_wndMenuOptionsAudio);
	uimenu_window_hide(mainmenu_wndMenuOptionsControls);
}

void mainmenu_show_options_audio(void)
{
	uimenu_window_hide(mainmenu_wndMenuOptionsGeneral);
	uimenu_window_hide(mainmenu_wndMenuOptionsVideo);
	uimenu_window_show(mainmenu_wndMenuOptionsAudio);
	uimenu_window_hide(mainmenu_wndMenuOptionsControls);
}

void mainmenu_show_options_controls(void)
{
	uimenu_window_hide(mainmenu_wndMenuOptionsGeneral);
	uimenu_window_hide(mainmenu_wndMenuOptionsVideo);
	uimenu_window_hide(mainmenu_wndMenuOptionsAudio);
	uimenu_window_show(mainmenu_wndMenuOptionsControls);
}


void mainmenu_show_options(void)
{
	uimenu_window_show(mainmenu_wndMenuOptions);
	mainmenu_show_options_general();
}
void mainmenu_hide_options(void)
{
	uimenu_window_hide(mainmenu_wndMenuOptions);
	uimenu_window_hide(mainmenu_wndMenuOptionsGeneral);
	uimenu_window_hide(mainmenu_wndMenuOptionsVideo);
	uimenu_window_hide(mainmenu_wndMenuOptionsAudio);
	uimenu_window_hide(mainmenu_wndMenuOptionsControls);
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
    mainmenu_current_response = 0;
	music_speedup(100);
	music_start("media/mainmenu.mp3", 0.5, 0);
	uimenu_window_show(mainmenu_wndMenuMain);
	uimenu_window_show(mainmenu_wndMenuBackground);
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
