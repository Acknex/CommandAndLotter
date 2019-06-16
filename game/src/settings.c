#include "settings.h"
#include "ini.h"

STRING * settings_file = "settings.ini";

#define CSIDL_APPDATA 0x001A
#define SHGFP_TYPE_CURRENT 0

HRESULT SHGetFolderPath(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPSTR pszPath);
#define PRAGMA_API SHGetFolderPath;shell32!SHGetFolderPathA

char settings_path[MAX_PATH];

void settings_init()
{
    // try to get "%APPDATA%\AckCon\2019\settings.ini"
    if(ExpandEnvironmentStrings("%APPDATA%", settings_path, MAX_PATH) == 0)
    {
        error("Failed to expand environment variable for settings.");
        return;
    }
    str_cpy(settings_file, settings_path);
    str_cat(settings_file, "\\AckCon");
    CreateDirectory(_chr(settings_file), NULL);
    str_cat(settings_file, "\\2019");
    CreateDirectory(_chr(settings_file), NULL);
    str_cat(settings_file, "\\settings.ini");

    diag("\nstore settings in ");
    diag(_chr(settings_file));

    settings.enable_shadows = true;
    settings.enable_bloom = true;
    settings.res_x = 1280;
    settings.res_y = 720;
    settings.max_framerate = 60;
    settings.master_volume = 100;


    if(!file_exists(settings_file)) {
        settings_save();
    }
}

void settings_load()
{
    if(!file_exists(settings_file)) {
        error("file not found!");
        return;
    }
    settings.enable_bloom = ini_read_int(settings_file, "generic", "bloom", 1);
    settings.enable_shadows = ini_read_int(settings_file, "generic", "shadows", 1);
    settings.max_framerate = ini_read_int(settings_file, "generic", "fps_limit", 60);
    settings.res_x = ini_read_int(settings_file, "resolution", "width", 1280);
    settings.res_y = ini_read_int(settings_file, "resolution", "height", 720);
    settings.master_volume = ini_read_int(settings_file, "volume", "master", 100);
}

void settings_save()
{
    ini_write_int(settings_file, "generic",    "bloom",     settings.enable_bloom);
    ini_write_int(settings_file, "generic",    "shadows",   settings.enable_shadows);
    ini_write_int(settings_file, "generic",    "fps_limit", settings.max_framerate);
    ini_write_int(settings_file, "resolution", "width",     settings.res_x);
    ini_write_int(settings_file, "resolution", "height",    settings.res_y);
    ini_write_int(settings_file, "volume",     "master",    settings.master_volume);
}
