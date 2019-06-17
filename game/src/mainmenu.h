#ifndef MAINMENU_H
#define MAINMENU_H

#include "global.h"

int mainmenu_current_response = 0;
var mainmenu_buzz_wait_time = 2;
var mainmenu_buzz_handle;

uimenu_window_t * mainmenu_wndMenuBackground;
uimenu_window_t * mainmenu_wndMenuBackgroundBright;
uimenu_window_t * mainmenu_wndMenuMain;
uimenu_element_t * mainmenu_btnNewGame;
uimenu_element_t * mainmenu_btnOptions;
uimenu_element_t * mainmenu_btnCredits;
uimenu_element_t * mainmenu_btnExit;
uimenu_window_t * mainmenu_wndMenuOptions;
uimenu_window_t * mainmenu_wndMenuOptionsGeneral;
uimenu_window_t * mainmenu_wndMenuOptionsVideo;
uimenu_window_t * mainmenu_wndMenuOptionsAudio;
uimenu_window_t * mainmenu_wndMenuOptionsControls;

SOUND *mainmenu_buzz = "bzzt.wav";

BMAP * mainmenu_btn_new_game = "NewGame.png";
BMAP * mainmenu_btn_options = "Options.png";
BMAP * mainmenu_btn_credits = "Credits.png";
BMAP * mainmenu_btn_exit = "exit.png";

BMAP * mainmenu_background_bright = "MenueBackground_fc.png";
BMAP * mainmenu_background = "MenueBackground_fc2.png";

#define MAINMENU_RESPONSE_KEEP      0
#define MAINMENU_RESPONSE_STARTGAME 1
#define MAINMENU_RESPONSE_CREDIT    2
#define MAINMENU_RESPONSE_EXIT      3

void mainmenu_init(void);
void mainmenu_open(void);
void mainmenu_update(void);
void mainmenu_resolution_update(void);
void mainmenu_close(void);

void mainmenu_set_start(void);
void mainmenu_show_options(void);
void mainmenu_show_options_general(void);
void mainmenu_show_options_video(void);
void mainmenu_show_options_audio(void);
void mainmenu_show_options_controls(void);
void mainmenu_hide_options(void);
void mainmenu_set_credits(void);
void mainmenu_set_exit(void);

void mainmenu_add_item();

//! returns
int mainmenu_get_response(void);

#endif // MAINMENU_H
