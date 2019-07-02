#ifndef SETTINGS_H
#define SETTINGS_H

#include "global.h"

struct settings_t
{
    bool enable_shadows;
    bool enable_bloom;
    int max_framerate;
    int res_x, res_y;
    int master_volume;
    int intro_skipmode;
};

#define SETTINGS_INTRO_NOSKIP   0
#define SETTINGS_INTRO_FASTSKIP 1
#define SETTINGS_INTRO_AUTOSKIP 2

struct settings_t settings;

void settings_init(void);

void settings_load(void);

void settings_save(void);

#endif // SETTINGS_H
