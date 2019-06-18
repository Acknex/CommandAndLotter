#include "ui_game.h"
#include "unit.h"
#include "spawner.h"
#include "progressbars.h"
#include "building.h"
#include "jps.h"
#include "fow.h"
#include "global.h"

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
	ui_command_group_status = !ui_command_group_status;
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
	var px = (16000 - abs(ent->y + 8000)) / 16000;
	var py = (16000 - abs(ent->x + 8000)) / 16000;
	var t1 = pan_setwindow(ui_minimap, counter, px * bmap_width(ui_mm), py * bmap_height(ui_mm), size, size, dot, 0, 0);
	if(!t1)
	{
		pan_setwindow(ui_minimap, 0, px * bmap_width(ui_mm) * scale_factor_x * mini_map_extra_scale_x, py * bmap_height(ui_mm) * scale_factor_x * mini_map_extra_scale_y, size, size, dot, 0, 0);
	}
}

void ui_add_camera_to_minimap()
{
	var px = (16000 - abs(camera->y + 8000)) / 16000;
	var py = (16000 - abs(camera->x + 8000)) / 16000;
	var t1 = pan_setneedle(ui_minimap, 1, px * bmap_width(ui_mm), py * bmap_height(ui_mm), ui_bmap_camera, 8, 16, 0, 0, 360, camera.pan );
	if(!t1)
	{
		pan_setneedle(ui_minimap, 0, px * bmap_width(ui_mm) * scale_factor_x * mini_map_extra_scale_x, py * bmap_height(ui_mm) * scale_factor_x * mini_map_extra_scale_y, ui_bmap_camera, 8, 16, 0, 0, 360, camera.pan );
	}
}

void ui_minimap_click(PANEL *panel)
{
	var mpx = mouse_pos.x - panel->pos_x - (bmap_width(ui_mm) * scale_factor_x * mini_map_extra_scale_x) / 2;
	var mpy = mouse_pos.y - panel->pos_y - (bmap_height(ui_mm) * scale_factor_x * mini_map_extra_scale_y) / 2;

	mpx /= (bmap_width(ui_mm) * scale_factor_x * mini_map_extra_scale_x);
	mpy /= (bmap_height(ui_mm) * scale_factor_x * mini_map_extra_scale_y);

	mpx = mpx * 16000 * -1;
	mpy = mpy * 16000 * -1;
	topdown_camera_set_pos(vector(mpy, mpx, camera.z));
}

void ui_game_init()
{
	scale_factor_x = screen_size.x / 1920;
	scale_factor_y = screen_size.y / 1080;

	ui_radial_delete = ui_create_radial_button(NULL, ui_destroy_event_sputnik);
	ui_radial_counter = pan_create("", 2);
	ui_radial_cbabe = ui_create_radial_button(ui_icon_cbabe, ui_spawn_event_sputnik);
	ui_radial_sputnik = ui_create_radial_button(ui_icon_sputnik, ui_spawn_event_sputnik);
	ui_radial_skull = ui_create_radial_button(ui_icon_skull, ui_spawn_event_sputnik);
	ui_radial_esel = ui_create_radial_button(ui_icon_esel, ui_spawn_event_sputnik);

	ui_main_resources = pan_create("", 99);
	ui_unit_meta = pan_create("", 99);
	ui_game_menu = pan_create("", 99);
	ui_portrait = pan_create("", 99);
	ui_minimap = pan_create("", 100);
	ui_monitor = pan_create("", 99);

	ui_radial_counter->bmap = ui_radial_n;
	ui_radial_counter->flags = CENTER_X | CENTER_Y | UNTOUCHABLE;
	pan_setwindow(ui_radial_counter, 0, -5, -4, 128, 128, ui_radial_progress, 0, 0);
	pan_setdigits(ui_radial_counter, 0, bmap_width(ui_radial_n) / 2, bmap_height(ui_radial_n) / 2 , "%.0f", ui_hud_font, 1, &a_dummy_var);

	pan_setbutton(ui_game_menu, 0, 0, 1, 151, ui_hide_button_p, ui_hide_button_n, ui_hide_button_o, ui_hide_button_n, ui_show_commando_groups, NULL, NULL);

	pan_setbutton(ui_game_menu, 0, 4, 15, 134, ui_icon_tower_o, ui_icon_tower, ui_icon_tower_o, ui_icon_tower, ui_place_building, NULL, NULL);
	pan_setbutton(ui_game_menu, 0, 4, 15, 242, ui_icon_farm_o, ui_icon_farm, ui_icon_farm_o, ui_icon_farm, ui_place_building, NULL, NULL);
	pan_setbutton(ui_game_menu, 0, 4, 15, 350, ui_icon_blank_o, ui_icon_blank, ui_icon_blank_o, ui_icon_blank, ui_place_building, NULL, NULL);
	pan_setbutton(ui_game_menu, 0, 4, 15, 458, ui_icon_bank_o, ui_icon_bank, ui_icon_bank_o, ui_icon_bank, ui_place_building, NULL, NULL);

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
	ui_unit_meta->pos_x = 3;

	ui_game_menu->bmap = ui_bmap_gamemenu;

	ui_main_resources->bmap = ui_bmap_resources;
	ui_main_resources->pos_x = 3;
	ui_main_resources->pos_y = 3;
	ui_main_resources->flags |= ARIGHT;
	pan_setdigits(ui_main_resources, 0, bmap_width(ui_bmap_resources) - 50, 13, "%.0f", ui_hud_font, 1, &a_stupid_var );

	ui_monitor->bmap = ui_bmap_monitor;
	ui_minimap->bmap = ui_mm;
	ui_minimap->event = ui_minimap_click;

}

void ui_game_open()
{
	ui_monitor->flags |= SHOW;
	ui_minimap->flags |= SHOW;
	ui_minimap->pos_x = 46;
	ui_minimap->pos_y = screen_size.y - (bmap_height(ui_bmap_monitor) + 14)  * scale_factor_x;

	ui_game_menu->pos_x = screen_size.x - bmap_width(ui_bmap_gamemenu) + 100;
	ui_game_menu->pos_y = screen_size.y * 0.15;

	ui_unit_meta->pos_y = screen_size.y - bmap_height(ui_bmap_units);
	ui_main_resources->flags |= SHOW;

	ui_game_menu->flags |= SHOW;
}

void ui_game_close()
{
	ui_main_resources->flags &= ~SHOW;
	ui_unit_meta->flags &= ~SHOW;
	ui_hide_radial(ui_radial_sputnik);
	ui_hide_radial(ui_radial_cbabe);
	ui_hide_radial(ui_radial_skull);
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

	ui_add_camera_to_minimap();

	if( buildingState() == -1 )
	{
		button_state(ui_game_menu, 2, 0);
		button_state(ui_game_menu, 3, 0);
		button_state(ui_game_menu, 4, 0);
		button_state(ui_game_menu, 5, 0);
	}

	var scale_factor_x = screen_size.x / 1920;
	var scale_factor_y = screen_size.y / 1080;


	ui_main_resources->scale_x = scale_factor_x;
	ui_main_resources->scale_y = scale_factor_x;

	ui_unit_meta->scale_x = scale_factor_x;
	ui_unit_meta->scale_y = scale_factor_x;

	ui_game_menu->scale_x = scale_factor_x;
	ui_game_menu->scale_y = scale_factor_x;

	ui_portrait->scale_x = scale_factor_x;
	ui_portrait->scale_y = scale_factor_x;

	ui_minimap->scale_x = scale_factor_x * mini_map_extra_scale_x;
	ui_minimap->scale_y = scale_factor_x * mini_map_extra_scale_y;

	ui_monitor->scale_x = scale_factor_x;
	ui_monitor->scale_y = scale_factor_x;
	ui_monitor->pos_y = screen_size.y - bmap_height(ui_bmap_monitor) * scale_factor_x;
	ui_minimap->pos_x = floor(46 * scale_factor_x + 0.5);
	ui_minimap->pos_y = screen_size.y - bmap_height(ui_mm) * scale_factor_x - 12 * scale_factor_x;// + 18;// * scale_factor_x;

	ui_lifebar_counter = 0;

	ENTITY * ent;
	int ui_has_ents = 0;
	int ui_has_building = 0;
	int ui_count_sputniks = 0;
	int ui_count_esel = 0;
	int ui_count_cbabes = 0;
	int ui_count_skull = 0;

	if( !ui_command_group_status )
	{
		ui_game_menu->pos_x = screen_size.x - bmap_width(ui_bmap_gamemenu) * scale_factor_x + 100 * scale_factor_x;
		ui_game_menu->pos_y = screen_size.y * 0.15 * scale_factor_x;
	}
	else
	{
		ui_game_menu->pos_x = screen_size.x - bmap_width(ui_bmap_gamemenu) * scale_factor_x;
		ui_game_menu->pos_y = screen_size.y * 0.15 * scale_factor_x;
	}

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
            if (tile->visibility == FOW_SCOUTED)
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
					ui_count_skull++;
					break;
					case UNIT_BABE:
					ui_count_cbabes++;
					break;
				}

				if( str_cmp(ent->type, "SPUTNIK.MDL") )
				{
					ui_count_sputniks++;
				}
				update_or_create_lifebar(ent);
			}
		}

        TILE* tile = mapGetTileFromVector(mapGetCurrent(), &ent->x);
        if(tile)
        {
            if (tile->visibility == FOW_SCOUTED)
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
            if (tile->visibility == FOW_SCOUTED)
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

		if( ui_count_sputniks >= ui_count_esel && ui_count_sputniks >= ui_count_cbabes && ui_count_sputniks >= ui_count_skull )
		{
			ui_max_type = UI_SPUTNIK;
			ui_active_portrait = ui_bmap_sputnik;
		}
		else if( ui_count_cbabes >= ui_count_esel && ui_count_cbabes >= ui_count_sputniks && ui_count_sputniks >= ui_count_skull )
		{
			ui_max_type = UI_CBABE;
			ui_active_portrait = ui_bmap_cbabe;
		}
		else if( ui_count_skull >= ui_count_esel && ui_count_skull >= ui_count_sputniks && ui_count_skull >= ui_count_cbabes )
		{
			ui_max_type = UI_SKULL;
			ui_active_portrait = ui_bmap_eye;
		}
		else if( ui_count_esel >= ui_count_skull && ui_count_esel >= ui_count_sputniks && ui_count_esel >= ui_count_cbabes )
		{
			ui_max_type = UI_ESEL;
			ui_active_portrait = ui_bmap_esel;
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
		ui_anim_state = UI_ANIM_RESTARTED;
		last_building = ui_active_building;
	}

	if( ui_max_type != ui_selected_max_type )
	{
		ui_anim_unit_state = UI_ANIM_UNIT_RESTARTED;
		ui_anim_state = UI_ANIM_OFF;
		ui_selected_max_type = ui_max_type;
	}

	if( !ui_selected_max_type)
	{
		ui_anim_unit_state = UI_ANIM_UNIT_OFF;
	}

	if( ui_anim_unit_state == UI_ANIM_UNIT_RESTARTED )
	{

		ui_switch_frame = 0;
		pan_setwindow(ui_unit_meta, 1, 408, 98, 196, 186, (ui_active_portrait)[0], 0, 0);
		ui_anim_unit_state = UI_ANIM_UNIT_PROGRESS;
	}
	else if( ui_anim_unit_state == UI_ANIM_UNIT_PROGRESS )
	{
		ui_switch_frame += time_step * UI_ANIM_UNIT_SPEED;
		ui_switch_frame %= 19;
		if ( ui_switch_frame >= 13 && ui_switch_frame < 18)
		{
			ui_active_portrait = ui_bmap_noise;
		}
		else if( ui_switch_frame >= 18 )
		{
			ui_anim_unit_state = UI_ANIM_UNIT_OFF;
		}

		pan_setwindow(ui_unit_meta, 1, 408, 98, 196, 186, (ui_active_portrait)[ui_frame_order[ui_switch_frame]], 0, 0);
	}
	else if( ui_anim_unit_state == UI_ANIM_UNIT_ON )
	{

	}
	else if ( ui_anim_unit_state == UI_ANIM_UNIT_OFF )
	{
		pan_setwindow(ui_unit_meta, 1, 0, 0, 0, 0, (ui_bmap_cbabe)[0], 0, 0);
		if( ui_unit_meta->flags & SHOW )
		{
			ui_minimap->pos_x = 412 * scale_factor_x;
			ui_minimap->pos_y = screen_size.y - (202 * scale_factor_x);
			mini_map_extra_scale_x = 0.69;
			mini_map_extra_scale_y = 0.58;
			ui_minimap->scale_x = scale_factor_x * mini_map_extra_scale_x;
			ui_minimap->scale_y = scale_factor_x * mini_map_extra_scale_y;
			ui_minimap->flags |= SHOW;
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
