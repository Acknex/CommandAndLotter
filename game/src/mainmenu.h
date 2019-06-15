#ifndef MAINMENU_H
#define MAINMENU_H

#include "global.h"

int mainmenu_current_response = 0;
uimenu_window_t * mainmenu_wndMenuBackground;
uimenu_window_t * mainmenu_wndMenuMain;
uimenu_element_t * mainmenu_btnNewGame;
uimenu_element_t * mainmenu_btnOptions;
uimenu_element_t * mainmenu_btnCredits;
uimenu_element_t * mainmenu_btnExit;
uimenu_window_t * mainmenu_wndMenuOptions;

BMAP * mainmenu_btn_new_game = "NewGame.png";
BMAP * mainmenu_btn_options = "Options.png";
BMAP * mainmenu_btn_credits = "Credits.png";
BMAP * mainmenu_btn_exit = "exit.png";

BMAP * mainmenu_background = "MenueBackground_fc.png";

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
void mainmenu_set_credits(void);
void mainmenu_set_exit(void);

void mainmenu_add_item();

//! returns
int mainmenu_get_response(void);

#endif // MAINMENU_H
