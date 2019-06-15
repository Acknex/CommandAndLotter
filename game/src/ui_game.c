#include "ui_game.h"
#include "unit.h"

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
	rad->size_x = bmap_width(ui_radial_n) * scale;
	rad->size_y = bmap_height(ui_radial_n) * scale;
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

PANEL* ui_create_radial_button(BMAP *initial_icon)
{
	PANEL *rad = pan_create("", 2);
	
	rad->size_x = bmap_width(ui_radial_n);
	rad->size_y = bmap_height(ui_radial_n);
	
	rad->scale_x = scale_factor_x;
	rad->scale_y = scale_factor_x;
	
	pan_setbutton(rad, 0, 0, 0, 0, ui_radial_o, ui_radial_n, ui_radial_o, ui_radial_n, NULL, NULL, NULL);
	
	PANEL *icon = pan_create("", 3);
	icon->bmap = initial_icon;
	icon->scale_x = scale_factor_x;
	icon->scale_y = scale_factor_x;
	icon->flags |= UNTOUCHABLE;
	
	
	rad->skill_x = handle(icon);
	ui_repos_radial(rad, 500, 500);
	return rad;
}


void update_or_create_lifebar(ENTITY *ent)
{
	var health = unit_getHealth(ent);
	var percent = 100 * health;
	
	VECTOR screen;
	vec_set(screen, ent.x);
	vec_to_screen(screen, camera);
	
	if(ui_life_indicator[ui_lifebar_counter])
	{
		pan_setwindow(ui_life_indicator[ui_lifebar_counter], 1, 0, 0, 100 - percent, 8, ui_bmap_dead_indicator, 0, 0);
		pan_setwindow(ui_life_indicator[ui_lifebar_counter], 2, 100 - percent, 0, percent, 8, ui_bmap_life_indicator, 0, 0);
		ui_life_indicator[ui_lifebar_counter]->pos_x = screen.x;
		ui_life_indicator[ui_lifebar_counter]->pos_y = screen.y;
		ui_life_indicator[ui_lifebar_counter]->flags |= SHOW;
	}
	else
	{
		ui_life_indicator[ui_lifebar_counter] = pan_create("", 4);
		pan_setwindow(ui_life_indicator[ui_lifebar_counter], 0, 0, 0, 100 - percent, 8, ui_bmap_dead_indicator, 0, 0);
		pan_setwindow(ui_life_indicator[ui_lifebar_counter], 0, 100 - percent, 0, percent, 8, ui_bmap_life_indicator, 0, 0);
		ui_life_indicator[ui_lifebar_counter]->pos_x = screen.x;
		ui_life_indicator[ui_lifebar_counter]->pos_y = screen.y;
		ui_life_indicator[ui_lifebar_counter]->flags |= SHOW;
	}
	
	ui_lifebar_counter++;
}


void ui_game_init()
{
	scale_factor_x = screen_size.x / 1920;
	scale_factor_y = screen_size.y / 1080;
	
	ui_radial_delete = ui_create_radial_button(NULL);
	ui_radial_counter = pan_create("", 2);
	ui_radial_cbabe = ui_create_radial_button(ui_icon_cbabe);
	ui_radial_sputnik = ui_create_radial_button(ui_icon_sputnik);
	ui_radial_skull = ui_create_radial_button(ui_icon_skull);
	ui_radial_esel = ui_create_radial_button(ui_icon_esel);
	
	ui_main_resources = pan_create("", 99);
	ui_unit_meta = pan_create("", 99);
	ui_game_menu = pan_create("", 99);
	ui_portrait = pan_create("", 99);
	ui_minimap = pan_create("", 99);
	
	ui_radial_counter->bmap = ui_radial_n;
	
	pan_setbutton(ui_game_menu, 0, 0, 1, 151, ui_hide_button_p, ui_hide_button_n, ui_hide_button_o, ui_hide_button_n, ui_show_commando_groups, NULL, NULL);
	
	ui_bmap_cbabe[0] = bmap_create("CbFace1.png");
	ui_bmap_cbabe[1] = bmap_create("CbFace2.png");
	ui_bmap_cbabe[2] = bmap_create("CbFace3.png");
	
	ui_bmap_sputnik[0] = bmap_create("Sputnik1.png");
	ui_bmap_sputnik[1] = bmap_create("Sputnik2.png");
	ui_bmap_sputnik[2] = bmap_create("Sputnik3.png");
	
	ui_bmap_noise[0] = bmap_create("Rauschen1.png");
	ui_bmap_noise[1] = bmap_create("Rauschen2.png");
	ui_bmap_noise[2] = bmap_create("Rauschen3.png");
	
	pan_setwindow(ui_unit_meta, 0, 0, 0, 0, 0, ui_bmap_cbabe[0], 0, 0);
	
	ui_bmap_dead_indicator = bmap_createblack(8, 8, 24);
	ui_bmap_life_indicator = bmap_createblack(8, 8, 24);
	
	bmap_fill(ui_bmap_dead_indicator, vector(0, 0, 255), 100);
	bmap_fill(ui_bmap_life_indicator, vector(0, 255, 0), 100);
	
	int i; for(i = 0; i < 200; i++)
	{
		ui_life_indicator[i] = pan_create("", 1);
		pan_setwindow(ui_life_indicator[i], 0, 0, 0, 0, 0, ui_bmap_dead_indicator, 0, 0);
		pan_setwindow(ui_life_indicator[i], 0, 0, 0, 0, 0, ui_bmap_life_indicator, 0, 0);
	}
	
	ui_unit_meta->bmap = ui_bmap_units;
	ui_unit_meta->pos_x = 3;
	
	ui_game_menu->bmap = ui_bmap_gamemenu;
	
	ui_main_resources->bmap = ui_bmap_resources;
	ui_main_resources->pos_x = 3;
	ui_main_resources->pos_y = 3;
	ui_main_resources->flags |= ARIGHT;
	pan_setdigits(ui_main_resources, 0, bmap_width(ui_bmap_resources) - 50, 13, "%.0f", ui_hud_font, 1, &a_stupid_var );
	
}

void ui_game_open()
{
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
}

void ui_game_update()
{

	scale_factor_x = screen_size.x / 1920;
	scale_factor_y = screen_size.y / 1080;

	BMAP* bmp = mapGetBitmap(NULL);
	if(bmp)
	{
		draw_quad(bmp,vector(400,0,0),NULL,NULL,vector(2,2,0),NULL,100,0);
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
	
	ui_minimap->scale_x = scale_factor_x;
	ui_minimap->scale_y = scale_factor_x;
	
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
	
	SUBSYSTEM_LOOP(ent, SUBSYSTEM_UNIT_MANAGEMENT)
	{
		if( ent->skill[39] )
		{
			ent->MAXHEALTH = 100;
			ent->HEALTH = integer(random(100));
			ui_has_ents = 1;
			if( str_cmp(ent->type, "sputnik.mdl") )
			{
				ui_count_sputniks++;
			}
			update_or_create_lifebar(ent);
		}
	}
	
	SUBSYSTEM_LOOP(ent, SUBSYSTEM_SPAWNER)
	{
		if( ent->skill[39] )
		{
			if( ent->group == GROUP_PLAYER_SPAWNER )
			{
				ui_has_building = 1;
				ui_active_building = ent;
			}
		}
	}
	
	if(ui_has_ents)
	{
		ui_unit_meta->flags |= SHOW;
		
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
			ui_active_portrait = ui_bmap_cbabe;
		}
		else if( ui_count_esel >= ui_count_skull && ui_count_esel >= ui_count_sputniks && ui_count_esel >= ui_count_cbabes )
		{
			ui_max_type = UI_ESEL;
			ui_active_portrait = ui_bmap_cbabe;
		}
	}
	else
	{
		ui_unit_meta->flags &= ~SHOW;
	}
	
	
	
	VECTOR screen;
	
	a_stupid_var += 1;
	a_stupid_var %= 40000;
	
	if( last_building != ui_active_building)
	{
		ui_anim_state = UI_ANIM_RESTARTED;
		ui_anim_unit_state = UI_ANIM_UNIT_OFF;
		ui_selected_max_type = 0;
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
			ui_active_portrait = NULL;
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
		
		ui_life_indicator[0]->pos_x = screen.x - 50;
		ui_life_indicator[0]->pos_y = screen.y - 40;
	}
	
	if( ui_anim_state == UI_ANIM_RESTARTED )
	{
		int i; for(i = 0; i < 200; i++)
		{
			ui_life_indicator[i]->flags &= ~SHOW;
		}
		
		pan_setwindow(ui_life_indicator[0], 1, 0, 0, 76, 8, ui_bmap_dead_indicator, 0, 0);
		pan_setwindow(ui_life_indicator[0], 2, 76, 0, 24, 8, ui_bmap_life_indicator, 0, 0);
		ui_life_indicator[0]->flags |= SHOW;
		
		ui_orbit_radial(ui_radial_cbabe,screen.x, screen.y, 270, 200);
		ui_orbit_radial(ui_radial_delete,screen.x, screen.y, 230, 200);
		ui_orbit_radial(ui_radial_counter,screen.x, screen.y, 190, 200);
		
		ui_show_radial(ui_radial_cbabe);
		ui_scale_radial(ui_radial_cbabe, 0.1);
		
		ui_show_radial(ui_radial_delete);
		ui_scale_radial(ui_radial_delete, 0.1);
		
		ui_radial_counter->flags |= SHOW;
		ui_radial_counter->scale_x = 0.1;
		ui_radial_counter->scale_y = 0.1;
		
		ui_anim_state = UI_ANIM_PROGRESS;
	} 
	else if( ui_anim_state == UI_ANIM_PROGRESS ) 
	{
		ui_orbit_radial(ui_radial_cbabe,screen.x, screen.y, 270, 200);
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
		
		ui_scale_radial(ui_radial_cbabe, ui_scale1);
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
		ui_orbit_radial(ui_radial_cbabe,screen.x, screen.y, 270, 200);
		ui_orbit_radial(ui_radial_delete,screen.x, screen.y, 230, 200);
		ui_orbit_radial(ui_radial_counter,screen.x, screen.y, 190, 200);
		
		ui_scale_radial(ui_radial_cbabe, scale_factor_x);
		ui_scale_radial(ui_radial_delete, scale_factor_x);
		
		ui_radial_counter->scale_x = scale_factor_x;
		ui_radial_counter->scale_y = scale_factor_x;
		
	}
	else if ( ui_anim_state == UI_ANIM_OFF )
	{
		ui_hide_radial(ui_radial_cbabe);
		ui_hide_radial(ui_radial_delete);
		ui_radial_counter->flags &= ~SHOW;
		
		int i; for(i = 0; i < 200; i++)
		{
			ui_life_indicator[i]->flags &= ~SHOW;
		}
		
		ui_scale1 = 0.1;
		ui_scale2 = 0.1;
		ui_scale3 = 0.1;
	}
}

var ui_game_isdone()
{
	
}
