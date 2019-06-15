#include "ui_game.h"

void ui_game_init()
{
	ui_radial_delete = pan_create("", 99);
	ui_radial_counter = pan_create("", 99);
	ui_radial_unit = pan_create("", 99);
	ui_main_resources = pan_create("", 99);
	ui_unit_meta = pan_create("", 99);
	ui_game_menu = pan_create("", 99);
	
	ui_bmap_dead_indicator = bmap_createblack(8, 8, 24);
	ui_bmap_life_indicator = bmap_createblack(8, 8, 24);
	
	bmap_fill(ui_bmap_dead_indicator, vector(0, 0, 255), 100);
	bmap_fill(ui_bmap_life_indicator, vector(0, 255, 0), 100);
	
	int i; for(i = 0; i < 200; i++)
	{
		ui_life_indicator[i] = pan_create("", 99);
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

void test_stuff()
{
	my.skill1 = 2;
}

void ui_game_open()
{
	ui_game_menu->pos_x = screen_size.x - bmap_width(ui_bmap_gamemenu);
	ui_game_menu->pos_y = screen_size.y * 0.15;
	ui_unit_meta->pos_y = screen_size.y - bmap_height(ui_bmap_units);
//	ui_main_resources->flags |= SHOW;
//	ui_unit_meta->flags |= SHOW;
//	ui_game_menu->flags |= SHOW;
	
	//	ONLY FOR TESTING - THIS IS CRAP
	mouse_mode = 4;
	mouse_range = 6000;
	int i; for(i = 0; i < 3; i++)
	{
		some_prototype_ents[i] = ent_create("sphere.mdl", vector(100, 100, 100), NULL);
		some_prototype_ents[i]->skill1 = 0;
		some_prototype_ents[i]->flags |= INVISIBLE;
	}
	some_prototype_ents[3] = ent_create("cube.mdl", vector(0, 0, 2000), NULL);
	some_prototype_ents[3]->emask = ENABLE_CLICK;
	some_prototype_ents[3]->event = test_stuff;
	some_prototype_ents[3]->skill1 = 0;
	some_prototype_ents[3]->flags |= INVISIBLE;
}

void ui_game_close()
{
	ui_main_resources->flags &= ~SHOW;
	ui_unit_meta->flags &= ~SHOW;
	
	
	//	Release this crap
	int i; for(i = 0; i < 4; i++)
	{
		ptr_remove(some_prototype_ents[i]);
	}
}



void ui_game_update()
{
	ui_game_menu->pos_x = screen_size.x - bmap_width(ui_bmap_gamemenu);
	ui_game_menu->pos_y = screen_size.y * 0.15;
	ui_unit_meta->pos_y = screen_size.y - bmap_height(ui_bmap_units) - 3;
	
	VECTOR screen;
	
	a_stupid_var += 1;
	a_stupid_var %= 40000;
	
	// This is only for testing - framework subsystems go here
	int i; for(i = 0; i < 4; i++)
	{
		if(some_prototype_ents[i]->skill1 == 2)
		{
			if( last_building != some_prototype_ents[i])
			{
				ui_anim_state = UI_ANIM_RESTARTED;
				
			} 
			last_building = some_prototype_ents[i];
		}
	}
	
	if(!last_building)
	{
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
//		ui_life_indicator[0]->flags |= SHOW;
		
		
//		ui_radial_unit->flags |= SHOW;
//		ui_radial_delete->flags |= SHOW;
//		ui_radial_counter->flags |= SHOW;
		
		ui_radial_unit->scale_x = ui_scale1;
		ui_radial_unit->scale_y = ui_scale1;
		ui_radial_delete->scale_x = ui_scale2;
		ui_radial_delete->scale_y = ui_scale2;
		ui_radial_counter->scale_x = ui_scale3;
		ui_radial_counter->scale_y = ui_scale3;
		
		ui_radial_unit->pos_x = screen.x + sinv(270) * 120;
		ui_radial_unit->pos_y = screen.y + cosv(270) * 120;
		
		ui_radial_delete->pos_x = screen.x + sinv(90) * 120;
		ui_radial_delete->pos_y = screen.y + cosv(90) * 120;
		
		ui_radial_counter->pos_x = screen.x + sinv(170) * 200;
		ui_radial_counter->pos_y = screen.y + cosv(170) * 200;
		
		ui_anim_state = UI_ANIM_PROGRESS;
	} 
	else if( ui_anim_state == UI_ANIM_PROGRESS ) 
	{
		ui_radial_unit->pos_x = screen.x + sinv(270) * 120;
		ui_radial_unit->pos_y = screen.y + cosv(270) * 120;
		
		ui_radial_delete->pos_x = screen.x + sinv(90) * 120;
		ui_radial_delete->pos_y = screen.y + cosv(90) * 120;
		
		ui_radial_counter->pos_x = screen.x + sinv(170) * 200;
		ui_radial_counter->pos_y = screen.y + cosv(170) * 200;
		
		if( ui_scale1 <= 1 ) 
		{
			ui_scale1 += time_step * UI_ANIM_SPEED;
		} 
		if( ui_scale1 >= 0.5 )
		{
			ui_scale2 += time_step * UI_ANIM_SPEED;
		}
		if( ui_scale2 >= 0.5 )
		{
			ui_scale3 += time_step * UI_ANIM_SPEED;
		}
		if (ui_scale1 >= 1) 
		{
			ui_scale1 = 1;
		}
		if (ui_scale2 >= 1) 
		{
			ui_scale2 = 1;
		}
		if (ui_scale3 >= 1) 
		{
			ui_scale3 = 1;
		}
		
		ui_radial_unit->scale_x = ui_scale1;
		ui_radial_unit->scale_y = ui_scale1;
		ui_radial_delete->scale_x = ui_scale2;
		ui_radial_delete->scale_y = ui_scale2;
		ui_radial_counter->scale_x = ui_scale3;
		ui_radial_counter->scale_y = ui_scale3;
		
		if( ui_scale1 == 1 && ui_scale2 == 1 && ui_scale3 == 1)
		{
			ui_anim_state = UI_ANIM_ON;
		} 
	}
	else if( ui_anim_state == UI_ANIM_ON ) 
	{
		ui_radial_unit->pos_x = screen.x + sinv(270) * 120;
		ui_radial_unit->pos_y = screen.y + cosv(270) * 120;
		
		ui_radial_delete->pos_x = screen.x + sinv(90) * 120;
		ui_radial_delete->pos_y = screen.y + cosv(90) * 120;
		
		ui_radial_counter->pos_x = screen.x + sinv(170) * 200;
		ui_radial_counter->pos_y = screen.y + cosv(170) * 200;
	}
	else if ( ui_anim_state == UI_ANIM_OFF )
	{
		ui_radial_unit->flags &= ~SHOW;
		ui_radial_delete->flags &= ~SHOW;
		ui_radial_counter->flags &= ~SHOW;
	}
}

var ui_game_isdone()
{
	
}
