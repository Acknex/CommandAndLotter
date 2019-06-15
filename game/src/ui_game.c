#include "ui_game.h"

void ui_show_commando_groups()
{
	ui_command_group_status = !ui_command_group_status;
}

void ui_game_init()
{
	ui_radial_delete = pan_create("", 1);
	ui_radial_counter = pan_create("", 1);
	ui_radial_unit = pan_create("", 1);
	ui_main_resources = pan_create("", 99);
	ui_unit_meta = pan_create("", 99);
	ui_game_menu = pan_create("", 99);
	ui_portrait = pan_create("", 99);
	ui_minimap = pan_create("", 99);
	
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
	
	ui_radial_unit->bmap = ui_radial_prototype1;
	ui_radial_delete->bmap = ui_radial_prototype1;
	ui_radial_counter->bmap = ui_radial_prototype2;
	
	ui_unit_meta->bmap = ui_bmap_units;
	ui_unit_meta->pos_x = 3;
	
	ui_game_menu->bmap = ui_bmap_gamemenu;
	
	ui_main_resources->bmap = ui_bmap_resources;
	ui_main_resources->pos_x = 3;
	ui_main_resources->pos_y = 3;
	ui_main_resources->flags |= ARIGHT;
	pan_setdigits(ui_main_resources, 0, bmap_width(ui_bmap_resources) - 50, 13, "%.0f", ui_hud_font, 1, &a_stupid_var );
	
	ui_radial_unit->size_x = 32;
	ui_radial_unit->size_y = 32;
	ui_radial_delete->size_x = 32;
	ui_radial_delete->size_y = 32;
	ui_radial_counter->size_x = 32;
	ui_radial_counter->size_y = 32;
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
	ENTITY * ent;
	int ui_has_ents = 0;
	int ui_count_sputniks = 0;
	int ui_count_esel = 0;
	int ui_count_cbabes = 0;
	int ui_count_skull = 0;
	
	if( !ui_command_group_status )
	{
		ui_game_menu->pos_x = screen_size.x - bmap_width(ui_bmap_gamemenu) + 100;
		ui_game_menu->pos_y = screen_size.y * 0.15;
	}
	else
	{
		ui_game_menu->pos_x = screen_size.x - bmap_width(ui_bmap_gamemenu);
		ui_game_menu->pos_y = screen_size.y * 0.15;
	}
	
	int ui_max_type = 0;
	
	SUBSYSTEM_LOOP(ent, SUBSYSTEM_UNIT_MANAGEMENT)
	{
		if( ent->skill[39] )
		{
			ui_has_ents = 1;
			if( str_cmp(ent->type, "sputnik.mdl") )
			{
				ui_count_sputniks++;
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
	
	ui_unit_meta->pos_y = screen_size.y - bmap_height(ui_bmap_units) - 3;
	
	VECTOR screen;
	
	a_stupid_var += 1;
	a_stupid_var %= 40000;
	
	// This is only for testing - framework subsystems go here
//	int i; for(i = 0; i < 4; i++)
//	{
//		if(some_prototype_ents[i]->skill1 == 2 )
//		{
//			if( last_building != some_prototype_ents[i])
//			{
//				ui_anim_state = UI_ANIM_RESTARTED;
//				ui_anim_unit_state = UI_ANIM_UNIT_OFF;
//			} 
//			last_building = some_prototype_ents[i];
//			if(main_unit) { main_unit->skill1 = 0; main_unit = NULL; }
//		}
		if( ui_max_type != ui_selected_max_type )
		{
			ui_anim_unit_state = UI_ANIM_UNIT_RESTARTED;
			ui_anim_state = UI_ANIM_OFF;
			ui_selected_max_type = ui_max_type;
		}		
//	}
	
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
	
	
//	if(!last_building)
//	{
//		ui_anim_state = UI_ANIM_OFF;
//	}
//	else
//	{
//		vec_set(screen, last_building.x);
//		vec_to_screen(screen, camera);
//		
//		ui_life_indicator[0]->pos_x = screen.x - 50;
//		ui_life_indicator[0]->pos_y = screen.y - 40;
//	}
//	
//	if( ui_anim_state == UI_ANIM_RESTARTED )
//	{
//		
//		int i; for(i = 0; i < 200; i++)
//		{
//			ui_life_indicator[i]->flags &= ~SHOW;
//		}
//		
//		pan_setwindow(ui_life_indicator[0], 1, 0, 0, 76, 8, ui_bmap_dead_indicator, 0, 0);
//		pan_setwindow(ui_life_indicator[0], 2, 76, 0, 24, 8, ui_bmap_life_indicator, 0, 0);
//		ui_life_indicator[0]->flags |= SHOW;
//		
//		ui_radial_unit->flags |= SHOW;
//		ui_radial_delete->flags |= SHOW;
//		ui_radial_counter->flags |= SHOW;
//		
//		ui_radial_unit->scale_x = ui_scale1;
//		ui_radial_unit->scale_y = ui_scale1;
//		ui_radial_delete->scale_x = ui_scale2;
//		ui_radial_delete->scale_y = ui_scale2;
//		ui_radial_counter->scale_x = ui_scale3;
//		ui_radial_counter->scale_y = ui_scale3;
//		
//		ui_radial_unit->pos_x = screen.x + sinv(270) * 120;
//		ui_radial_unit->pos_y = screen.y + cosv(270) * 120;
//		
//		ui_radial_delete->pos_x = screen.x + sinv(90) * 120;
//		ui_radial_delete->pos_y = screen.y + cosv(90) * 120;
//		
//		ui_radial_counter->pos_x = screen.x + sinv(170) * 200;
//		ui_radial_counter->pos_y = screen.y + cosv(170) * 200;
//		
//		ui_anim_state = UI_ANIM_PROGRESS;
//	} 
//	else if( ui_anim_state == UI_ANIM_PROGRESS ) 
//	{
//		ui_radial_unit->pos_x = screen.x + sinv(270) * 120;
//		ui_radial_unit->pos_y = screen.y + cosv(270) * 120;
//		
//		ui_radial_delete->pos_x = screen.x + sinv(90) * 120;
//		ui_radial_delete->pos_y = screen.y + cosv(90) * 120;
//		
//		ui_radial_counter->pos_x = screen.x + sinv(170) * 200;
//		ui_radial_counter->pos_y = screen.y + cosv(170) * 200;
//		
//		if( ui_scale1 <= 1 ) 
//		{
//			ui_scale1 += time_step * UI_ANIM_SPEED;
//		} 
//		if( ui_scale1 >= 0.5 )
//		{
//			ui_scale2 += time_step * UI_ANIM_SPEED;
//		}
//		if( ui_scale2 >= 0.5 )
//		{
//			ui_scale3 += time_step * UI_ANIM_SPEED;
//		}
//		if (ui_scale1 >= 1) 
//		{
//			ui_scale1 = 1;
//		}
//		if (ui_scale2 >= 1) 
//		{
//			ui_scale2 = 1;
//		}
//		if (ui_scale3 >= 1) 
//		{
//			ui_scale3 = 1;
//		}
//		
//		ui_radial_unit->scale_x = ui_scale1;
//		ui_radial_unit->scale_y = ui_scale1;
//		ui_radial_delete->scale_x = ui_scale2;
//		ui_radial_delete->scale_y = ui_scale2;
//		ui_radial_counter->scale_x = ui_scale3;
//		ui_radial_counter->scale_y = ui_scale3;
//		
//		if( ui_scale1 == 1 && ui_scale2 == 1 && ui_scale3 == 1)
//		{
//			ui_anim_state = UI_ANIM_ON;
//		} 
//	}
//	else if( ui_anim_state == UI_ANIM_ON ) 
//	{
//		ui_radial_unit->pos_x = screen.x + sinv(270) * 120;
//		ui_radial_unit->pos_y = screen.y + cosv(270) * 120;
//		
//		ui_radial_delete->pos_x = screen.x + sinv(90) * 120;
//		ui_radial_delete->pos_y = screen.y + cosv(90) * 120;
//		
//		ui_radial_counter->pos_x = screen.x + sinv(170) * 200;
//		ui_radial_counter->pos_y = screen.y + cosv(170) * 200;
//		
//		
//	}
//	else if ( ui_anim_state == UI_ANIM_OFF )
//	{
//		ui_radial_unit->flags &= ~SHOW;
//		ui_radial_delete->flags &= ~SHOW;
//		ui_radial_counter->flags &= ~SHOW;
//		int i; for(i = 0; i < 200; i++)
//		{
//			ui_life_indicator[i]->flags &= ~SHOW;
//		}
//		ui_scale1 = 0.1;
//		ui_scale2 = 0.1;
//		ui_scale3 = 0.1;
//	}
}

var ui_game_isdone()
{
	
}
