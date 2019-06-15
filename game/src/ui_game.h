#ifndef UI_GAME
#define UI_GAME


#define UI_ANIM_OFF 0
#define UI_ANIM_PROGRESS 1
#define UI_ANIM_ON 2
#define UI_ANIM_RESTARTED 3
#define UI_ANIM_SPEED 0.3

#define UI_ANIM_UNIT_OFF 0
#define UI_ANIM_UNIT_PROGRESS 1
#define UI_ANIM_UNIT_ON 2
#define UI_ANIM_UNIT_RESTARTED 3
#define UI_ANIM_UNIT_SPEED 0.4

#define UI_SPUTNIK 1
#define UI_CBABE 2
#define UI_SKULL 3
#define UI_ESEL 4


PANEL *ui_main_resources;
PANEL *ui_unit_meta;
PANEL *ui_game_menu;

PANEL *ui_radial_delete;
PANEL *ui_radial_counter;
PANEL *ui_radial_cbabe;
PANEL *ui_radial_skull;
PANEL *ui_radial_esel;
PANEL *ui_radial_sputnik;

PANEL *ui_life_indicator[2000];

PANEL *ui_portrait;
PANEL *ui_minimap;

BMAP *ui_radial_prototype1 = "ui_placeholder_unit.png";
BMAP *ui_radial_prototype2 = "ui_placeholder_counter.png";

BMAP *ui_bmap_resources = "h3.png";
BMAP *ui_bmap_units = "h2.png";
BMAP *ui_bmap_gamemenu = "h1.png";

BMAP *ui_bmap_dead_indicator;
BMAP *ui_bmap_life_indicator;

BMAP *ui_bmap_cbabe[3];
BMAP *ui_bmap_sputnik[4];
BMAP *ui_bmap_noise[3];

BMAP **ui_active_portrait;

BMAP *ui_hide_button_n = "hide_button_n.png";
BMAP *ui_hide_button_o = "hide_button_o.png";
BMAP *ui_hide_button_p = "hide_button_p.png";

BMAP *ui_radial_o = "ui_placeholder_counter.png";
BMAP *ui_radial_n = "ui_placeholder_unit.png";

BMAP *ui_icon_cbabe = "ui_H2.png";
BMAP *ui_icon_sputnik = "ui_H1.png";
BMAP *ui_icon_skull = "ui_H4.png";
BMAP *ui_icon_esel = "ui_H3.png";

ENTITY* last_building;
ENTITY* main_unit;

ENTITY *ui_active;
ENTITY *ui_active_building;

FONT *ui_hud_font = "HUD_font.png";

var ui_anim_state = 0;
var ui_anim_unit_state = 0;
var ui_scale1 = 0.1;
var ui_scale2 = 0.1;
var ui_scale3 = 0.1;
var ui_switch_frame = 0;
var a_stupid_var = 1234;
var a_dummy_var = 0;

int ui_frame_order[19] = {0, 1, 0, 1, 2, 1, 0, 1, 1, 2, 0, 2, 0, 0, 1, 2, 0, 1, 2};
int ui_selected_max_type = 0;
int ui_command_group_status = 0;
int ui_lifebar_counter = 0;


var scale_factor_x = 1;
var scale_factor_y = 1;

void ui_game_init();
void ui_game_open();
void ui_game_close();
void ui_game_update();
var ui_game_isdone();


#endif