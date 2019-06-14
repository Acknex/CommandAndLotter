#ifndef GAME_H
#define GAME_H

#include "global.h"

// Ingame Playstate

void game_init(void);

void game_open(void);
void game_update(void);
void game_close(void);

bool game_is_done(void);

#endif // GAME_H
