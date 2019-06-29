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
#define UI_EYE 3
#define UI_ESEL 4
#define UI_SKULL 5


PANEL *ui_main_resources;
PANEL *ui_unit_meta;
PANEL *ui_game_menu;
PANEL *ui_monitor;

PANEL *ui_radial_delete;
PANEL *ui_radial_counter;
PANEL *ui_radial_cbabe;
PANEL *ui_radial_skull;
PANEL *ui_radial_esel;
PANEL *ui_radial_sputnik;
PANEL *ui_radial_active;

PANEL *ui_life_indicator[2000];

PANEL *ui_portrait;
PANEL *ui_minimap;

BMAP *ui_radial_prototype1 = "ui_placeholder_unit.png";
BMAP *ui_radial_prototype2 = "ui_placeholder_counter.png";

BMAP *ui_bmap_resources = "H3.png";
BMAP *ui_bmap_units = "H2.png";
BMAP *ui_bmap_gamemenu = "H1.png";

BMAP *ui_bmap_dead_indicator;
BMAP *ui_bmap_life_indicator;
BMAP *ui_bmap_yellow = "#2x2x24";
BMAP *ui_bmap_magenta = "#2x2x24";
BMAP *ui_bmap_cyan = "#2x2x24";
BMAP *ui_bmap_red = "#2x2x24";
BMAP *ui_bmap_green = "#2x2x24";
BMAP *ui_bmap_blue = "#2x2x24";
BMAP *ui_bmap_camera = "minimap_cam_marker.png";

BMAP *ui_bmap_cbabe[3];
BMAP *ui_bmap_sputnik[3];
BMAP *ui_bmap_noise[3];
BMAP *ui_bmap_eye[3];
BMAP *ui_bmap_esel[3];
BMAP *ui_bmap_skull[8];

BMAP **ui_active_portrait;

BMAP *ui_hide_button_n = "hide_button_n.png";
BMAP *ui_hide_button_o = "hide_button_o.png";
BMAP *ui_hide_button_p = "hide_button_p.png";

BMAP *ui_radial_o = "ui_placeholder_counter.png";
BMAP *ui_radial_n = "ui_placeholder_unit.png";
BMAP *ui_radial_progress = "#128x128x8888";

BMAP *ui_icon_cbabe = "ui_H2.png";
BMAP *ui_icon_sputnik = "ui_H1.png";
BMAP *ui_icon_skull = "ui_H4.png";
BMAP *ui_icon_esel = "ui_H3.png";

BMAP *ui_icon_tree = "build_menu_tree_n.png";
BMAP *ui_icon_tree_o = "build_menu_tree_o.png";
BMAP *ui_icon_press = "build_menu_press_n.png";
BMAP *ui_icon_press_o = "build_menu_press_o.png";
BMAP *ui_icon_farm = "build_menu_esel_n.png";
BMAP *ui_icon_farm_o = "build_menu_esel_o.png";
BMAP *ui_icon_bank = "build_menu_bank_n.png";
BMAP *ui_icon_bank_o = "build_menu_bank_o.png";
BMAP *ui_icon_tower = "build_menu_tower_n.png";
BMAP *ui_icon_tower_o = "build_menu_tower_o.png";

BMAP *ui_face_cbabe1 = "CbFace1.png";
BMAP *ui_face_cbabe2 = "CbFace2.png";
BMAP *ui_face_cbabe3 = "CbFace3.png";

BMAP *ui_face_sputnik1 = "Sputnik1.png";
BMAP *ui_face_sputnik2 = "Sputnik2.png";
BMAP *ui_face_sputnik3 = "Sputnik3.png";

BMAP *ui_face_esel1 = "ESFace1.png";
BMAP *ui_face_esel2 = "ESFace2.png";
BMAP *ui_face_esel3 = "ESFace3.png";

BMAP *ui_face_eye1 = "ui_H4.png";
BMAP *ui_face_eye2 = "ui_H4.png";
BMAP *ui_face_eye3 = "ui_H4.png";

BMAP *ui_face_skull1 = "WkFace1.png";
BMAP *ui_face_skull2 = "WkFace2.png";
BMAP *ui_face_skull3 = "WkFace3.png";
BMAP *ui_face_skull4 = "WkFace4.png";
BMAP *ui_face_skull5 = "WkFace5.png";
BMAP *ui_face_skull6 = "WkFace6.png";
BMAP *ui_face_skull7 = "WkFace7.png";
BMAP *ui_face_skull8 = "WkFace8.png";

BMAP *ui_face_noise1 = "Rauschen1.png";
BMAP *ui_face_noise2 = "Rauschen2.png";
BMAP *ui_face_noise3 = "Rauschen3.png";

BMAP *ui_mm = NULL;
BMAP *ui_bmap_monitor = "monitor.png";

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
var mini_map_extra_scale_x = 1;
var mini_map_extra_scale_y = 1;

var ui_map_shift_x = 410;
var ui_map_shift_y = 96;
var ui_map_offset_x = 0;
var ui_map_offset_y = 0;


void ui_game_init();
void ui_game_open();
void ui_game_close();
void ui_game_update();
var ui_game_isdone();
void ui_game_after_all();

#endif
