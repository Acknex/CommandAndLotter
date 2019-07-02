#include "ui_game.h"
#include "unit.h"
#include "spawner.h"
#include "progressbars.h"
#include "building.h"
#include "jps.h"
#include "fow.h"
#include "global.h"

#define UI_MMAP_SCALE 5

void ui_spawn_event_sputnik(var num, PANEL *panel)
{
	ENTITY *ent = panel->skill_y;
	if (!ent) { return; } // NOOOO
	spawner_produce(ent);
	ent->SELCTED_SKILL = 1;
}

void ui_destroy_event_sputnik(var num, PANEL *panel)
{
	ENTITY *ent= panel->skill_y;
	if (!ent) { return; } // NOOOO
	ent->HEALTH = 0;
	ent->ENTITY_STATE = 4;
}

void ui_place_building(var num, PANEL *panel)
{
	buildingPlacement_beginConstruction(num - 2);
}

void ui_show_commando_groups()
{
	switch(ui_command_group_status)
	{
		case 0:
		ui_command_group_status = 1;
		ui_command_group_process = 0;
		break;
		case 1:
		ui_command_group_status = -1;
		break;
		case 2:
		ui_command_group_status = -1;
		ui_command_group_process = 0;
		break;
		case -1:
		ui_command_group_status = 1;
		break;
		default:
		ui_command_group_status = 0;
		ui_command_group_process = 0;
	}
}

void ui_rescale_radial(PANEL *rad)
{
	PANEL *icon = ptr_for_handle(rad->skill_x);
	rad->scale_x = 1;
	rad->scale_y = 1;
	rad->size_x = bmap_width(ui_radial_n);
	rad->size_y = bmap_height(ui_radial_n);
	rad->scale_x = scale_factor_x;
	rad->scale_y = scale_factor_x;
	icon->scale_x = scale_factor_x;
	icon->scale_y = scale_factor_x;
}

void ui_scale_radial(PANEL *rad, var scale)
{
	PANEL *icon = ptr_for_handle(rad->skill_x);
	rad->size_x = bmap_width(ui_radial_n);
	rad->size_y = bmap_height(ui_radial_n);
	rad->scale_x = scale;
	rad->scale_y = scale;
	icon->scale_x = scale;
	icon->scale_y = scale;
}

void ui_orbit_radial(PANEL *rad, int x, int y, var angle, var dist)
{
	VECTOR res;
	rad->pos_x = x + sinv(angle) * dist;
	rad->pos_y = y + cosv(angle) * dist;
	if(rad->skill_x)
	{
		PANEL *icon = ptr_for_handle(rad->skill_x);
		icon->pos_x = rad->pos_x;
		icon->pos_y = rad->pos_y;
	}
}

void ui_show_radial(PANEL *rad)
{
	PANEL *icon = ptr_for_handle(rad->skill_x);
	icon->flags |= SHOW;
	rad->flags |= SHOW;
}

void ui_hide_radial(PANEL *rad)
{
	if(!rad) { return; }
	PANEL *icon = ptr_for_handle(rad->skill_x);

	icon->flags &= ~SHOW;
	rad->flags &= ~SHOW;
}

void ui_repos_radial(PANEL *rad, int x, int y)
{
	PANEL *icon = ptr_for_handle(rad->skill_x);
	icon->pos_x = x;
	icon->pos_y = y;
	rad->pos_x = x;
	rad->pos_y = y;
}

PANEL* ui_create_radial_button(BMAP *initial_icon, void *ev)
{
	PANEL *rad = pan_create("", 2);

	rad->size_x = bmap_width(ui_radial_n);
	rad->size_y = bmap_height(ui_radial_n);

	pan_setbutton(rad, 0, 0, 0, 0, ui_radial_o, ui_radial_n, ui_radial_o, ui_radial_n, ev, NULL, NULL);

	rad->scale_x = scale_factor_x;
	rad->scale_y = scale_factor_x;

	PANEL *icon = pan_create("", 3);
	icon->bmap = initial_icon;
	icon->scale_x = scale_factor_x;
	icon->scale_y = scale_factor_x;
	icon->flags |= UNTOUCHABLE;


	rad->skill_x = handle(icon);
	return rad;
}

void update_or_create_lifebar(ENTITY *ent)
{
	if( ui_lifebar_counter >= 2000 ) { return; }

	var health = unit_getHealth(ent);
	var fac = health;

	VECTOR sc;
	vec_set(sc, ent.x);
	vec_to_screen(sc, camera);
	vec_sub(sc, vector(15, 35, 0));

	var p1 = 30 * (1 - fac);
	var p2 = 30 * fac;

	if(ui_life_indicator[ui_lifebar_counter])
	{
		pan_setwindow(ui_life_indicator[ui_lifebar_counter], 1, 0, 0, p1, 2, ui_bmap_dead_indicator, 0, 0);
		pan_setwindow(ui_life_indicator[ui_lifebar_counter], 2, p1, 0, p2, 2, ui_bmap_life_indicator, 0, 0);
	}
	else
	{
		ui_life_indicator[ui_lifebar_counter] = pan_create("", 1);
		pan_setwindow(ui_life_indicator[ui_lifebar_counter], 0, 0, 0, p1, 2, ui_bmap_dead_indicator, 0, 0);
		pan_setwindow(ui_life_indicator[ui_lifebar_counter], 0, p1, 0, p2, 2, ui_bmap_life_indicator, 0, 0);
	}

	ui_life_indicator[ui_lifebar_counter]->skill_x = handle(ent);
	ui_life_indicator[ui_lifebar_counter]->pos_x = sc.x;
	ui_life_indicator[ui_lifebar_counter]->pos_y = sc.y;
	ui_life_indicator[ui_lifebar_counter]->alpha = 60;
	ui_life_indicator[ui_lifebar_counter]->flags |= (SHOW | UNTOUCHABLE | TRANSLUCENT);

	ui_lifebar_counter++;
}

void ui_add_dot_to_minimap(ENTITY *ent, int size, BMAP *dot, int counter)
{
	//	var px = (16000 - abs(ent->y + 8000)) / 16000;
	//	var py = (16000 - abs(ent->x + 8000)) / 16000;
	//	var t1 = pan_setwindow(ui_minimap, counter, px * bmap_width(ui_mm), py * bmap_height(ui_mm), size, size, dot, 0, 0);
	//	if(!t1)
	//	{
		//		pan_setwindow(ui_minimap, 0, px * bmap_width(ui_mm) * scale_factor_x * mini_map_extra_scale_x, py * bmap_height(ui_mm) * scale_factor_x * mini_map_extra_scale_y, size, size, dot, 0, 0);
	//	}
}

void ui_add_camera_to_minimap()
{
	//	var px = (16000 - abs(camera->y + 8000)) / 16000;
	//	var py = (16000 - abs(camera->x + 8000)) / 16000;
	//	var t1 = pan_setneedle(ui_minimap, 1, px * bmap_width(ui_mm), py * bmap_height(ui_mm), ui_bmap_camera, 8, 16, 0, 0, 360, camera.pan );
	//	if(!t1)
	//	{
		//		pan_setneedle(ui_minimap, 0, px * bmap_width(ui_mm) * scale_factor_x * mini_map_extra_scale_x, py * bmap_height(ui_mm) * scale_factor_x * mini_map_extra_scale_y, ui_bmap_camera, 8, 16, 0, 0, 360, camera.pan );
	//	}
}

void ui_minimap_click(PANEL *panel)
{
	var mpx = mouse_pos.x - panel->pos_x - (bmap_width(ui_mm) * scale_factor_x * mini_map_extra_scale_x) / 2;
	var mpy = mouse_pos.y - panel->pos_y - (bmap_height(ui_mm) * scale_factor_x * mini_map_extra_scale_y) / 2;

	mpx /= (bmap_width(ui_mm) * scale_factor_x * mini_map_extra_scale_x);
	mpy /= (bmap_height(ui_mm) * scale_factor_x * mini_map_extra_scale_y);

    var aspect = bmap_width(ui_mm) / bmap_height(ui_mm);

	mpx = mpx * 16000;
	mpy = mpy * 16000 / aspect;
	topdown_camera_set_pos(vector(mpx, mpy, camera.z));
}

void ui_game_init()
{
	scale_factor_x = screen_size.x / 1920;
	scale_factor_y = screen_size.y / 1080;

	ui_radial_delete = ui_create_radial_button(ui_icon_explo, ui_destroy_event_sputnik);
	ui_radial_counter = pan_create("", 2);
	ui_radial_cbabe = ui_create_radial_button(ui_icon_cbabe, ui_spawn_event_sputnik);
	ui_radial_sputnik = ui_create_radial_button(ui_icon_sputnik, ui_spawn_event_sputnik);
	ui_radial_eye = ui_create_radial_button(ui_icon_eye, ui_spawn_event_sputnik);
	ui_radial_skull = ui_create_radial_button(ui_icon_skull, ui_spawn_event_sputnik);
	ui_radial_esel = ui_create_radial_button(ui_icon_esel, ui_spawn_event_sputnik);

	ui_main_resources = pan_create("", 99);
	ui_unit_meta = pan_create("", 99);
	ui_game_menu = pan_create("", 99);
	ui_portrait = pan_create("", 99);
	ui_minimap = pan_create("", 100);
	ui_monitor = pan_create("", 99);
	ui_open_game_menu = pan_create("", 101);

	ui_radial_counter->bmap = ui_radial_n;
	ui_radial_counter->flags = CENTER_X | CENTER_Y | UNTOUCHABLE;
	pan_setwindow(ui_radial_counter, 0, -5, -4, 128, 128, ui_radial_progress, 0, 0);
	pan_setdigits(ui_radial_counter, 0, bmap_width(ui_radial_n) / 2, bmap_height(ui_radial_n) / 2 , "%.0f", ui_hud_font, 1, &a_dummy_var);


	pan_setbutton(ui_game_menu, 0, 0, 1, 151, ui_hide_button_p, ui_hide_button_n, ui_hide_button_o, ui_hide_button_n, ui_show_commando_groups, NULL, NULL);

	pan_setbutton(ui_game_menu, 0, 4, 15, 134, ui_icon_press_o, ui_icon_press, ui_icon_press_o, ui_icon_press, ui_place_building, NULL, NULL);
	pan_setbutton(ui_game_menu, 0, 4, 15, 242, ui_icon_farm_o, ui_icon_farm, ui_icon_farm_o, ui_icon_farm, ui_place_building, NULL, NULL);
	pan_setbutton(ui_game_menu, 0, 4, 15, 350, ui_icon_tree_o, ui_icon_tree, ui_icon_tree_o, ui_icon_tree, ui_place_building, NULL, NULL);
	pan_setbutton(ui_game_menu, 0, 4, 15, 458, ui_icon_tower_o, ui_icon_tower, ui_icon_tower_o, ui_icon_tower, ui_place_building, NULL, NULL);
	pan_setbutton(ui_game_menu, 0, 4, 15, 564, ui_icon_bank_o, ui_icon_bank, ui_icon_bank_o, ui_icon_bank, ui_place_building, NULL, NULL);

	ui_open_game_menu->bmap = ui_icon_hammer;
	pan_setbutton(ui_open_game_menu, 0, 0, 0, 0, ui_icon_hammer_o, ui_icon_hammer, ui_icon_hammer_o, NULL, ui_show_commando_groups, NULL, NULL);

	ui_bmap_cbabe[0] = ui_face_cbabe1;
	ui_bmap_cbabe[1] = ui_face_cbabe2;
	ui_bmap_cbabe[2] = ui_face_cbabe3;

	ui_bmap_sputnik[0] = ui_face_sputnik1;
	ui_bmap_sputnik[1] = ui_face_sputnik2;
	ui_bmap_sputnik[2] = ui_face_sputnik3;

	ui_bmap_noise[0] = ui_face_noise1;
	ui_bmap_noise[1] = ui_face_noise2;
	ui_bmap_noise[2] = ui_face_noise3;

	ui_bmap_esel[0] = ui_face_esel1;
	ui_bmap_esel[1] = ui_face_esel2;
	ui_bmap_esel[2] = ui_face_esel3;

	ui_bmap_eye[0] = ui_face_eye1;
	ui_bmap_eye[1] = ui_face_eye2;
	ui_bmap_eye[2] = ui_face_eye3;

	ui_bmap_skull[0] = ui_face_skull1;
	ui_bmap_skull[1] = ui_face_skull2;
	ui_bmap_skull[2] = ui_face_skull3;
	ui_bmap_skull[3] = ui_face_skull4;
	ui_bmap_skull[4] = ui_face_skull5;
	ui_bmap_skull[5] = ui_face_skull6;
	ui_bmap_skull[6] = ui_face_skull7;
	ui_bmap_skull[7] = ui_face_skull8;

	pan_setwindow(ui_unit_meta, 0, 0, 0, 0, 0, ui_bmap_cbabe[0], 0, 0);

	ui_bmap_dead_indicator = bmap_createblack(8, 8, 24);
	ui_bmap_life_indicator = bmap_createblack(8, 8, 24);

	bmap_fill(ui_bmap_dead_indicator, vector(0, 0, 255), 100);
	bmap_fill(ui_bmap_life_indicator, vector(0, 255, 0), 100);

	bmap_fill(ui_bmap_red, vector(0, 0, 255), 100);
	bmap_fill(ui_bmap_green, vector(0, 255, 0), 100);
	bmap_fill(ui_bmap_blue, vector(255, 0, 0), 100);
	bmap_fill(ui_bmap_yellow, vector(0, 255, 255), 100);
	bmap_fill(ui_bmap_cyan, vector(255, 255, 0), 100);
	bmap_fill(ui_bmap_magenta, vector(255, 0, 255), 100);

	ui_unit_meta->bmap = ui_bmap_units;
	ui_unit_meta->pos_x = 0;

	pan_setwindow(ui_unit_meta, 0, 649, 187, 0, 0, ui_name_sputnik, 0, 0);
	pan_setwindow(ui_unit_meta, 0, 649, 187, 0, 0, ui_name_esel, 0, 0);
	pan_setwindow(ui_unit_meta, 0, 649, 187, 0, 0, ui_name_eye, 0, 0);
	pan_setwindow(ui_unit_meta, 0, 649, 187, 0, 0, ui_name_cbabe, 0, 0);
	pan_setwindow(ui_unit_meta, 0, 649, 187, 0, 0, ui_name_skull, 0, 0);

	ui_game_menu->bmap = ui_bmap_gamemenu;

	ui_main_resources->bmap = ui_bmap_resources;
	ui_main_resources->pos_x = 3;
	ui_main_resources->pos_y = 3;
	ui_main_resources->flags |= ARIGHT;
	pan_setdigits(ui_main_resources, 0, bmap_width(ui_bmap_resources) - 50, 13, "%.0f", ui_hud_font, 1, &a_stupid_var );

	ui_monitor->bmap = ui_bmap_monitor;

	ui_minimap->event = ui_minimap_click;
}

void ui_game_open()
{
	MAP * map = mapGetCurrent();
	if(map == NULL)
	error("ui_game_open must be called after jps init!");

	if(ui_mm != NULL)
	error("doppelter aufruf von ui_game_open");
	ui_mm = bmap_createblack(UI_MMAP_SCALE * map->size[0], UI_MMAP_SCALE * map->size[1], 8888);
	if(ui_mm == NULL)
	error("konnte ui_mm nicht erstellen!");
	ui_minimap->bmap = ui_mm;

	ui_monitor->flags |= SHOW;
	ui_minimap->flags |= SHOW;
	ui_minimap->pos_x = 46;
	ui_minimap->pos_y = screen_size.y - (bmap_height(ui_bmap_monitor) + 14)  * scale_factor_x;

	ui_game_menu->pos_x = screen_size.x - bmap_width(ui_bmap_gamemenu) + 100;
	ui_game_menu->pos_y = screen_size.y * 0.15;

	ui_unit_meta->pos_y = screen_size.y - bmap_height(ui_bmap_units);
	ui_main_resources->flags |= SHOW;

	pan_setwindow(ui_unit_meta, 2, 649, 187, 0, 0, ui_name_sputnik, 0, 0);

	ui_game_menu->flags &= ~SHOW;

	ui_open_game_menu->flags |= SHOW;
}

void ui_game_close()
{
	ui_main_resources->flags &= ~SHOW;
	ui_unit_meta->flags &= ~SHOW;
	ui_hide_radial(ui_radial_sputnik);
	ui_hide_radial(ui_radial_cbabe);
	ui_hide_radial(ui_radial_skull);
	ui_hide_radial(ui_radial_eye);
	ui_hide_radial(ui_radial_esel);
	ui_game_menu->flags &= ~SHOW;
	ui_radial_counter->flags &= ~SHOW;
	ui_portrait->flags &= ~SHOW;
	ui_minimap->flags &= ~SHOW;
	ui_radial_delete->flags &= ~SHOW;
	ui_monitor->flags &= ~SHOW;

	int i; for(i = 0; i < 2000; i++)
	{
		if( ui_life_indicator[i] )
		{
			ui_life_indicator[i]->flags &= ~SHOW;
		}
	}

	ptr_remove(ui_mm);
	ui_mm = NULL;
}

// skaliert *dst* so, dass es in *rect* relativ zu *dst* zentriert ist.
// übernimmt hierbei die skalierung von *ref*
void ui_game_scale_panel_to_rect(PANEL * dst, PANEL * ref, VECTOR * pos, VECTOR * size)
{
    var dst_w = bmap_width(dst->bmap);
    var dst_h = bmap_height(dst->bmap);
    dst->scale_x = ref->scale_x * size->x / dst_w;
    dst->scale_y = ref->scale_y * size->y / dst_h;

    var scale = minv(dst->scale_x, dst->scale_y);
    dst->scale_x = scale;
    dst->scale_y = scale;

    dst->pos_x = ref->pos_x + ref->scale_x * pos->x + (ref->scale_x * size->x - scale * dst_w) / 2;
    dst->pos_y = ref->pos_y + ref->scale_y * pos->y + (ref->scale_x * size->y - scale * dst_h) / 2;
}

void ui_game_update()
{
	int i; for(i = 0; i < ui_lifebar_counter; i++)
	{
		ENTITY *ent = ptr_for_handle(ui_life_indicator[i]->skill_x);
		if(ent)
		{
			if( !ent->SELCTED_SKILL )
			{
				ui_life_indicator[i]->flags &= ~SHOW;
			}
		}
	}

	scale_factor_x = screen_size.x / 1920;
	scale_factor_y = screen_size.y / 1080;

	if(key_m)
	{
		BMAP* bmp = mapGetBitmap(NULL);
		if(bmp)
		{
			int size = 4;
			draw_quad(bmp,vector(000,0,0),NULL,NULL,vector(size,size,0),NULL,100,0);
			int unitDrawSize = 4;
			int currentPlayer;
			for(currentPlayer = 0; currentPlayer < MAX_PLAYERS; currentPlayer++)
			{
				UNIT *unit = mapCurrent.unitFirst[currentPlayer];
				while(unit)
				{
					if(unit->isActive)
					{
						draw_quad(NULL,vector(unit->pos2d.x*size-unitDrawSize/2,unit->pos2d.y*size-unitDrawSize/2,0),NULL,vector(4,4,0),NULL,COLOR_RED,50,0);
					}
					unit = unit->next;
				}
			}
		}
	}

	// ui_add_camera_to_minimap();

	if( buildingState() == -1 )
	{
		button_state(ui_game_menu, 2, 0);
		button_state(ui_game_menu, 3, 0);
		button_state(ui_game_menu, 4, 0);
		button_state(ui_game_menu, 5, 0);
		button_state(ui_game_menu, 6, 0);
	}

	var scale_factor_x = screen_size.x / 1920;
	var scale_factor_y = screen_size.y / 1080;


	ui_main_resources->scale_x = scale_factor_x;
	ui_main_resources->scale_y = scale_factor_x;

	ui_unit_meta->scale_x = scale_factor_x;
	ui_unit_meta->scale_y = scale_factor_x;

	ui_portrait->scale_x = scale_factor_x;
	ui_portrait->scale_y = scale_factor_x;

	ui_monitor->scale_x = scale_factor_x;
	ui_monitor->scale_y = scale_factor_x;
	ui_monitor->pos_y = screen_size.y - bmap_height(ui_bmap_monitor) * scale_factor_x;


	// ui_minimap->scale_x = scale_factor_x * mini_map_extra_scale_x;
	// ui_minimap->scale_y = scale_factor_x * mini_map_extra_scale_y;
	// ui_minimap->pos_x = floor(46 * scale_factor_x + 0.5);
	// ui_minimap->pos_y = screen_size.y - bmap_height(ui_mm) * scale_factor_x - 12 * scale_factor_x;// + 18;// * scale_factor_x;

    // improved minimap placement
    ui_game_scale_panel_to_rect(ui_minimap, ui_monitor, vector(64, 33, 0), vector(253, 284, 0));

	ui_game_menu->scale_x = scale_factor_x;
	ui_game_menu->scale_y = scale_factor_x;

	ui_open_game_menu->scale_x = scale_factor_x;
	ui_open_game_menu->scale_y = scale_factor_x;
	ui_open_game_menu->pos_x = screen_size.x - bmap_width(ui_icon_hammer) * scale_factor_x;
	ui_open_game_menu->pos_y = screen_size.y * 0.15 * scale_factor_x + 10 * scale_factor_x;

	ui_lifebar_counter = 0;

	ENTITY * ent;
	int ui_has_ents = 0;
	int ui_has_building = 0;
	int ui_count_sputniks = 0;
	int ui_count_esel = 0;
	int ui_count_cbabes = 0;
	int ui_count_eye = 0;
	int ui_count_skull = 0;

	if(ui_command_group_status == 1 || ui_command_group_status == -1)
	{
		ui_game_menu->flags |= SHOW;
		ui_open_game_menu->flags |= SHOW;

		ui_command_group_process += 25 * time_step;

		if( ui_command_group_status == 1 )
		{
			ui_open_game_menu->alpha = clamp(100 - ui_command_group_process, 0, 100);
			ui_game_menu->alpha = clamp(ui_command_group_process, 0, 100);
		}
		else if ( ui_command_group_status == -1 )
		{
			ui_open_game_menu->alpha = clamp(ui_command_group_process, 0, 100);
			ui_game_menu->alpha = clamp(100 - ui_command_group_process, 0, 100);
		}

		if ( ui_command_group_process >= 100 )
		{
			if( ui_command_group_status == 1)
			{
				ui_command_group_process = 0;
				ui_command_group_status = 2;
				ui_game_menu->flags |= SHOW;
				ui_open_game_menu->flags &= ~SHOW;
			}
			if( ui_command_group_status == -1)
			{
				ui_command_group_process = 0;
				ui_command_group_status = 0;
				ui_game_menu->flags &= ~SHOW;
				ui_open_game_menu->flags |= SHOW;
			}
		}
	}

	ui_game_menu->pos_x = screen_size.x - bmap_width(ui_bmap_gamemenu) * scale_factor_x;
	ui_game_menu->pos_y = screen_size.y * 0.15 * scale_factor_x;

	ui_unit_meta->pos_y = screen_size.y - bmap_height(ui_bmap_units) * scale_factor_x - 3;

	int ui_max_type = 0;
	int counter = 1;
	SUBSYSTEM_LOOP(ent, SUBSYSTEM_SPAWNER)
	{
		if( ent->SELCTED_SKILL && ent->HEALTH > 0 )
		{
			if( ent->group == GROUP_PLAYER_SPAWNER )
			{
				ui_has_building = 1;
				ui_active_building = ent;

				update_or_create_lifebar(ent);

				switch(ent->ENTITY_UNITTYPE)
				{
					case UNIT_SPUTNIK:
					ui_radial_active = ui_radial_sputnik;
					ui_radial_sputnik->skill_y = 0;
					break;
					case UNIT_LERCHE:
					ui_radial_active = ui_radial_esel;
					ui_radial_esel->skill_y = 0;
					break;
					case UNIT_EYE:
					ui_radial_active = ui_radial_eye;
					ui_radial_eye->skill_y = 0;
					break;
					case UNIT_SKULL: //TODO: add me properly
					ui_radial_active = ui_radial_skull;
					ui_radial_skull->skill_y = 0;
					break;
					case UNIT_BABE:
					ui_radial_cbabe->skill_y = 0;
					ui_radial_active = ui_radial_cbabe;
					break;
				}

				ui_radial_active->skill_y = ent;
				ui_radial_delete->skill_y = ent;
				ui_radial_counter->skill_y = ent;

				a_dummy_var = spawner_getQueue(ent);
			}
		}

		TILE* tile = mapGetTileFromVector(mapGetCurrent(), &ent->x);
		if(tile)
		{
			if (fow_isVisible(tile))
			{
				if( ent->group == GROUP_ENEMY_SPAWNER )
				{
					ui_add_dot_to_minimap(ent, 3, ui_bmap_green, counter);
					counter++;
					} else {
					ui_add_dot_to_minimap(ent, 3, ui_bmap_red, counter);
					counter++;
				}
			}
		}
	}
	for(ent = ent_next(NULL); ent != NULL; ent = ent_next(ent))
	{
		if( ent->group == GROUP_PLAYER_UNIT)
		{
			if( ent->SELCTED_SKILL )
			{
				ui_has_ents = 1;

				switch(unit_getType(ent))
				{
					case UNIT_SPUTNIK:
					ui_count_sputniks++;
					break;
					case UNIT_LERCHE:
					ui_count_esel++;
					break;
					case UNIT_EYE:
					ui_count_eye++;
					break;
					case UNIT_BABE:
					ui_count_cbabes++;
					break;
					case UNIT_SKULL:
					ui_count_skull++;
					break;
				}

				update_or_create_lifebar(ent);
			}
		}

		TILE* tile = mapGetTileFromVector(mapGetCurrent(), &ent->x);
		if(tile)
		{
			if (fow_isVisible(tile))
			{
				if(ent->group == GROUP_PLAYER_UNIT)
				{
					ui_add_dot_to_minimap(ent, 1, ui_bmap_cyan, counter);
					counter++;
				}
				else if( ent->group == GROUP_ENEMY_UNIT && ent->ENTITY_UNITTYPE != UNIT_Z)
				{
					ui_add_dot_to_minimap(ent, 1, ui_bmap_red, counter);
					counter++;
				}
			}
		}
	}

	SUBSYSTEM_LOOP(ent, SUBSYSTEM_Z)
	{
		TILE* tile = mapGetTileFromVector(mapGetCurrent(), &ent->x);
		if(tile)
		{
			if (fow_isVisible(tile))
			{
				ui_add_dot_to_minimap(ent, 1, ui_bmap_yellow, counter);
				counter++;
			}
		}
	}

	if(ui_has_ents)
	{
		ui_unit_meta->flags |= SHOW;
		ui_monitor->flags &= ~SHOW;
		ui_minimap->flags &= ~SHOW;


		int mask = 0;
		if(ui_count_sputniks > 0) mask |= 1;
		if(ui_count_cbabes > 0)   mask |= 2;
		if(ui_count_eye > 0)      mask |= 4;
		if(ui_count_esel > 0)     mask |= 8;
		if(ui_count_skull > 0)    mask |= 16;

		int i; for(i = 0; i < 5; i++)
		{
			if((mask & (1 << i)) == 0)
			continue;
			ui_max_type = i + 1;
			break;
		}

		if(mask != 0)
		{
			switch(ui_max_type)
			{
				case UI_SPUTNIK:
				ui_active_portrait = ui_bmap_sputnik;
				ui_frame_order = ui_frame_order_norm;
				pan_setwindow(ui_unit_meta, 2, 649 + (324 - bmap_width(ui_name_sputnik)) / 2, 195 + (65 - bmap_height(ui_name_sputnik)) / 2, bmap_width(ui_name_sputnik), bmap_height(ui_name_sputnik), ui_name_sputnik, 0, 0);
				break;
				case UI_CBABE:
				ui_active_portrait = ui_bmap_cbabe;
				ui_frame_order = ui_frame_order_norm;
				pan_setwindow(ui_unit_meta, 2, 649 + (324 - bmap_width(ui_name_cbabe)) / 2, 195 + (65 - bmap_height(ui_name_cbabe)) / 2, bmap_width(ui_name_cbabe), bmap_height(ui_name_cbabe), ui_name_cbabe, 0, 0);
				break;
				case UI_EYE:
				ui_active_portrait = ui_bmap_eye;
				ui_frame_order = ui_frame_order_norm;
				pan_setwindow(ui_unit_meta, 2, 649 + (324 - bmap_width(ui_name_eye)) / 2, 195 + (65 - bmap_height(ui_name_eye)) / 2, bmap_width(ui_name_eye), bmap_height(ui_name_eye), ui_name_eye, 0, 0);
				break;
				case UI_ESEL:
				ui_active_portrait = ui_bmap_esel;
				ui_frame_order = ui_frame_order_norm;
				pan_setwindow(ui_unit_meta, 2, 649 + (324 - bmap_width(ui_name_esel)) / 2, 195 + (65 - bmap_height(ui_name_esel)) / 2, bmap_width(ui_name_esel), bmap_height(ui_name_esel), ui_name_esel, 0, 0);
				break;
				case UI_SKULL:
				ui_active_portrait = ui_bmap_skull;
				ui_frame_order = ui_frame_order_spceial;
				pan_setwindow(ui_unit_meta, 2, 649 + (324 - bmap_width(ui_name_skull)) / 2, 195 + (65 - bmap_height(ui_name_skull)) / 2, bmap_width(ui_name_skull), bmap_height(ui_name_skull), ui_name_skull, 0, 0);
				break;
			}
		}
	}
	else
	{
		ui_unit_meta->flags &= ~SHOW;
		ui_monitor->flags |= SHOW;
		ui_minimap->flags |= SHOW;
	}



	VECTOR screen;

	a_stupid_var = z_get(PLAYER_ID_PLAYER);


	if( last_building != ui_active_building)
	{
		ui_hide_radial(ui_radial_sputnik);
		ui_hide_radial(ui_radial_cbabe);
		ui_hide_radial(ui_radial_skull);
		ui_hide_radial(ui_radial_esel);
		ui_hide_radial(ui_radial_eye);
		ui_anim_state = UI_ANIM_RESTARTED;
		last_building = ui_active_building;
	}

	if( ui_max_type != ui_selected_max_type )
	{
		ui_anim_unit_state = UI_ANIM_UNIT_RESTARTED;
		//ui_anim_state = UI_ANIM_OFF;
		ui_selected_max_type = ui_max_type;
	}

	if( !ui_selected_max_type)
	{
		ui_anim_unit_state = UI_ANIM_UNIT_OFF;
	}

	if( ui_anim_unit_state == UI_ANIM_UNIT_RESTARTED )
	{
		ui_anim_state = UI_ANIM_OFF;
		ui_switch_frame = 0;
		pan_setwindow(ui_unit_meta, 1, 408, 98, 196, 186, (ui_active_portrait)[0], 0, 0);
		ui_anim_unit_state = UI_ANIM_UNIT_PROGRESS;
	}
	else if( ui_anim_unit_state == UI_ANIM_UNIT_PROGRESS )
	{
		// Frame-Count + immer gleiche Reihenfolge noise mit 3 frames
		var anim_frame_count = (ui_frame_order)[0] + 3;
		ui_switch_frame += time_step * UI_ANIM_UNIT_SPEED;
		var ui_active_frame = integer(ui_switch_frame);

		if( ui_active_frame >= anim_frame_count ) // Terminieren
		{
			ui_anim_unit_state = UI_ANIM_UNIT_OFF;
		}
		else if ( ui_active_frame >= anim_frame_count - 3 ) // noise
		{
			ui_active_portrait = ui_bmap_noise;
			pan_setwindow(ui_unit_meta, 1, 408, 98, 196, 186, (ui_active_portrait)[ui_frame_noise_order[ ui_switch_frame - (ui_frame_order)[0] ]], 0, 0);
		}
		else // Normale Animation
		{
			pan_setwindow(ui_unit_meta, 1, 408, 98, 196, 186, (ui_active_portrait)[(ui_frame_order)[ui_active_frame + 1]], 0, 0);
		}
	}
	else if( ui_anim_unit_state == UI_ANIM_UNIT_ON )
	{

	}
	else if ( ui_anim_unit_state == UI_ANIM_UNIT_OFF )
	{
		pan_setwindow(ui_unit_meta, 1, 0, 0, 0, 0, (ui_bmap_cbabe)[0], 0, 0);
		if( ui_unit_meta->flags & SHOW )
		{
			// ui_minimap->pos_x = 412 * scale_factor_x;
			// ui_minimap->pos_y = screen_size.y - (202 * scale_factor_x);
			mini_map_extra_scale_x = 0.69;
			mini_map_extra_scale_y = 0.58;
			// ui_minimap->scale_x = scale_factor_x * mini_map_extra_scale_x;
			// ui_minimap->scale_y = scale_factor_x * mini_map_extra_scale_y;
			ui_minimap->flags |= SHOW;

            ui_game_scale_panel_to_rect(ui_minimap, ui_unit_meta, vector(421, 110, 0), vector(172, 162, 0));
		}
		else
		{
			mini_map_extra_scale_x = 1;
			mini_map_extra_scale_y = 1;
		}
	}


	if(!last_building || !ui_has_building)
	{
		last_building = NULL;
		ui_anim_state = UI_ANIM_OFF;
	}
	else
	{
		vec_set(screen, last_building.x);
		vec_to_screen(screen, camera);
	}

	if( ui_radial_active )
	{
		if( ui_anim_state == UI_ANIM_RESTARTED )
		{
			ui_orbit_radial(ui_radial_active,screen.x, screen.y, 270, 200);
			ui_orbit_radial(ui_radial_delete,screen.x, screen.y, 230, 200);
			ui_orbit_radial(ui_radial_counter,screen.x, screen.y, 190, 200);

			ui_anim_unit_state = UI_ANIM_UNIT_OFF;

			ui_show_radial(ui_radial_active);
			ui_scale_radial(ui_radial_active, 0.1);

			ui_show_radial(ui_radial_delete);
			ui_scale_radial(ui_radial_delete, 0.1);

			ui_radial_counter->flags |= SHOW;
			ui_radial_counter->scale_x = 0.1;
			ui_radial_counter->scale_y = 0.1;

			ui_anim_state = UI_ANIM_PROGRESS;
		}
		else if( ui_anim_state == UI_ANIM_PROGRESS )
		{
			ui_orbit_radial(ui_radial_active,screen.x, screen.y, 270, 200);
			ui_orbit_radial(ui_radial_delete,screen.x, screen.y, 230, 200);
			ui_orbit_radial(ui_radial_counter,screen.x, screen.y, 190, 200);

			if( ui_scale1 <= scale_factor_x )
			{
				ui_scale1 += time_step * UI_ANIM_SPEED;
			}
			if( ui_scale1 >= 0.5 * scale_factor_x )
			{
				ui_scale2 += time_step * UI_ANIM_SPEED;
			}
			if( ui_scale2 >= 0.5* scale_factor_x )
			{
				ui_scale3 += time_step * UI_ANIM_SPEED;
			}

			if (ui_scale1 >= scale_factor_x)
			{
				ui_scale1 = scale_factor_x;
			}
			if (ui_scale2 >= scale_factor_x)
			{
				ui_scale2 = scale_factor_x;
			}
			if (ui_scale3 >= scale_factor_x)
			{
				ui_scale3 = scale_factor_x;
			}

			ui_scale_radial(ui_radial_active, ui_scale1);
			ui_scale_radial(ui_radial_delete, ui_scale2);

			ui_radial_counter->scale_x = ui_scale3;
			ui_radial_counter->scale_y = ui_scale3;

			if( ui_scale1 == scale_factor_x && ui_scale2 == scale_factor_x && ui_scale3 == scale_factor_x)
			{
				ui_anim_state = UI_ANIM_ON;
			}
		}
		else if( ui_anim_state == UI_ANIM_ON )
		{
			ui_orbit_radial(ui_radial_active,screen.x, screen.y, 270, 200);
			ui_orbit_radial(ui_radial_delete,screen.x, screen.y, 230, 200);
			ui_orbit_radial(ui_radial_counter,screen.x, screen.y, 190, 200);

			ui_scale_radial(ui_radial_active, scale_factor_x);
			ui_scale_radial(ui_radial_delete, scale_factor_x);

			ui_radial_counter->scale_x = scale_factor_x;
			ui_radial_counter->scale_y = scale_factor_x;


		}
		else if ( ui_anim_state == UI_ANIM_OFF )
		{
			ui_hide_radial(ui_radial_active);
			ui_hide_radial(ui_radial_delete);
			ui_radial_counter->flags &= ~SHOW;

			ui_scale1 = 0.1;
			ui_scale2 = 0.1;
			ui_scale3 = 0.1;
		}

	}

	// render minimap
	{
		var fmt = bmap_lock(ui_mm, 0);

		MAP * map = mapGetCurrent();
		if(map == NULL) error("ui_game_update requires a map!");

		var col_fogged = pixel_for_vec(vector(  0,   0,   0),   0, fmt);
		var col_wall   = pixel_for_vec(vector( 64,  64,  96), 100, fmt);
		var col_ground = pixel_for_vec(vector(128, 128, 128), 100, fmt);

		var col_player[2];
		col_player[PLAYER_ID_PLAYER]  = pixel_for_vec(PLAYER_COLOR_PLAYER, 100, fmt);
		col_player[PLAYER_ID_AI]      = pixel_for_vec(PLAYER_COLOR_AI, 100, fmt);

		int y; for(y = 0; y < map->size[1]; y++)
		{
			int x; for(x = 0; x < map->size[0]; x++)
			{
				TILE * tile = mapTileGet(map, x, y);
				var col = col_fogged;
				if(fow_isVisible(tile))
				{
					if(tile->value)
					col = col_wall;
					else
					col = col_ground;
				}

				int dy; for(dy = 0; dy < UI_MMAP_SCALE; dy++)
				{
					int dx; for(dx = 0; dx < UI_MMAP_SCALE; dx++)
					{
						pixel_to_bmap(ui_mm, UI_MMAP_SCALE * x + dx, UI_MMAP_SCALE * y + dy, col);
					}
				}
			}
		}

		int currentPlayer;
		for(currentPlayer = 0; currentPlayer < MAX_PLAYERS; currentPlayer++)
		{
			UNIT * unit = map.unitFirst[currentPlayer];
			while(unit)
			{
				if(unit->isActive && unit->tile)
				{
					if(fow_isVisible(unit->tile))
					{
						pixel_to_bmap(ui_mm, UI_MMAP_SCALE * unit->pos2d.x+0, UI_MMAP_SCALE * unit->pos2d.y+0, col_player[currentPlayer]);
						pixel_to_bmap(ui_mm, UI_MMAP_SCALE * unit->pos2d.x+1, UI_MMAP_SCALE * unit->pos2d.y+0, col_player[currentPlayer]);
						pixel_to_bmap(ui_mm, UI_MMAP_SCALE * unit->pos2d.x+0, UI_MMAP_SCALE * unit->pos2d.y+1, col_player[currentPlayer]);
						pixel_to_bmap(ui_mm, UI_MMAP_SCALE * unit->pos2d.x+1, UI_MMAP_SCALE * unit->pos2d.y+1, col_player[currentPlayer]);
					}
				}
				unit = unit->next;
			}
		}

		// TODO: Add camera here

		bmap_unlock(ui_mm);
	}
}

var ui_game_isdone()
{

}

void ui_game_after_all()
{
	ENTITY * ent = ui_radial_counter->skill_y;
	if(ent)
	progressbar_radial_render(ui_radial_progress, 100 - 100 * spawner_getProgress(ent), 60, vector(0, 1, 0), 50);
}
