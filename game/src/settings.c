#include "settings.h"
#include "ini.h"

#define SETTINGS_FILE "settings.ini"

void settings_init()
{
    settings.enable_shadows = true;
    settings.enable_bloom = true;
    settings.res_x = 1280;
    settings.res_y = 720;
    settings.max_framerate = 60;
}

void settings_load()
{
    if(!file_exists(SETTINGS_FILE))
        return;
    settings.enable_bloom = ini_read_int(SETTINGS_FILE, "generic", "bloom", 1);
    settings.enable_shadows = ini_read_int(SETTINGS_FILE, "generic", "shadows", 1);
    settings.max_framerate = ini_read_int(SETTINGS_FILE, "generic", "fps_limit", 60);
    settings.res_x = ini_read_int(SETTINGS_FILE, "resolution", "width", 1280);
    settings.res_y = ini_read_int(SETTINGS_FILE, "resolution", "height", 720);
}

void settings_save()
{
    ini_write_int(SETTINGS_FILE, "generic",    "bloom",     settings.enable_bloom);
    ini_write_int(SETTINGS_FILE, "generic",    "shadows",   settings.enable_shadows);
    ini_write_int(SETTINGS_FILE, "generic",    "fps_limit", settings.max_framerate);
    ini_write_int(SETTINGS_FILE, "resolution", "width",     settings.res_x);
    ini_write_int(SETTINGS_FILE, "resolution", "height",    settings.res_y);
}
