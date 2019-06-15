#ifndef MAINMENU_H
#define MAINMENU_H

#include "global.h"

int mainmenu_current_response = 0;

BMAP * mainmenu_btn_new_game = "btn_menu_new_game.png";
BMAP * mainmenu_btn_new_game_hi = "btn_menu_new_game_hi.png";
BMAP * mainmenu_btn_options = "btn_menu_options.png";
BMAP * mainmenu_btn_options_hi = "btn_menu_options_hi.png";
BMAP * mainmenu_btn_credits = "btn_menu_credits.png";
BMAP * mainmenu_btn_credits_hi = "btn_menu_credits_hi.png";
BMAP * mainmenu_btn_exit = "btn_menu_exit.png";
BMAP * mainmenu_btn_exit_hi = "btn_menu_exit_hi.png";

#define MAINMENU_RESPONSE_KEEP      0
#define MAINMENU_RESPONSE_STARTGAME 1
#define MAINMENU_RESPONSE_CREDIT    2
#define MAINMENU_RESPONSE_EXIT      3

void mainmenu_init(void);
void mainmenu_open(void);
void mainmenu_update(void);
void mainmenu_close(void);

void mainmenu_set_start(void);
void mainmenu_set_credits(void);
void mainmenu_set_exit(void);

void mainmenu_add_item();

//! returns
int mainmenu_get_response(void);

#endif // MAINMENU_H
