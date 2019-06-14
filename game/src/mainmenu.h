#ifndef MAINMENU_H
#define MAINMENU_H

#include "global.h"

#define MAINMENU_RESPONSE_KEEP      0
#define MAINMENU_RESPONSE_STARTGAME 1
#define MAINMENU_RESPONSE_CREDIT    2
#define MAINMENU_RESPONSE_EXIT      3

void mainmenu_init(void);
void mainmenu_open(void);
void mainmenu_update(void);
void mainmenu_close(void);

//! returns
int mainmenu_get_response(void);

#endif // MAINMENU_H
