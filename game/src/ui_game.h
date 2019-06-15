#ifndef UI_GAME
#define UI_GAME

#define UI_ANIM_OFF 0
#define UI_ANIM_PROGRESS 1
#define UI_ANIM_ON 2
#define UI_ANIM_RESTARTED 3
#define UI_ANIM_SPEED 0.3

PANEL *ui_main_resources;
PANEL *ui_unit_meta;
PANEL *ui_game_menu;

PANEL *ui_radial_delete;
PANEL *ui_radial_counter;
PANEL *ui_radial_unit;

PANEL *ui_life_indicator[200];

var a_stupid_var = 1234;

BMAP *ui_radial_prototype1 = "ui_placeholder_unit.png";
BMAP *ui_radial_prototype2 = "ui_placeholder_counter.png";

BMAP *ui_bmap_resources = "h3.png";
BMAP *ui_bmap_units = "h2.png";
BMAP *ui_bmap_gamemenu = "h1.png";

BMAP *ui_bmap_dead_indicator;
BMAP *ui_bmap_life_indicator;

ENTITY* some_prototype_ents[4];
ENTITY* last_building;

FONT *ui_hud_font = "HUD_font.png";

var ui_anim_state = 0;
var ui_scale1 = 0.1;
var ui_scale2 = 0.1;
var ui_scale3 = 0.1;

void ui_game_init();
void ui_game_open();
void ui_game_close();
void ui_game_update();
var ui_game_isdone();


#endif