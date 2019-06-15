#include "global.h"
#include "framework.h"
#include "z.h"
#include "map_loader.h"

#define Z_VALUE 10

SOUND* z_collect_snd = "z_collect.wav";
var z_amount = 0;

var z_get()
{
	return z_amount;
}

ENTITY* z_spawn(VECTOR* pos)
{
	ENTITY* ent;
	ent = ent_create(CUBE_MDL, pos, Z);
	ent->group = GROUP_ENEMY_UNIT; //should be neutral group
	return ent;
}

void Z()
{
	framework_setup(my, SUBSYSTEM_Z);
	set(my, SHADOW);
	vec_scale(my->scale_x, 10); //temp
	c_setminmax(me);
	my->ENTITY_STATE = ENTITY_STATE_WAIT_OR_WALK;	
}

void Z_Init()
{
	z_amount = 0;
}

void Z_Update()
{
	ENTITY * ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_Z)
	{
			
		if (ptr->DAMAGE_HIT > 0 && ptr->ENTITY_STATE != ENTITY_STATE_DIE && ptr->ENTITY_STATE != ENTITY_STATE_DEAD)
      	{
      		Z__hit(ptr);
		}
		
		switch(ptr->ENTITY_STATE)    	
		{

			case ENTITY_STATE_WAIT_OR_WALK:
			{
				Z__wait(ptr);
				break;
			}

			case ENTITY_STATE_DIE:
			{
				Z__die(ptr);
				break;
			}

			default:
			{
				break;
			}
		}	
	
		ptr->z = maploader_get_height(ptr->x) - ptr->min_z+200;			
		
	}	
}

void Z__hit(ENTITY* ptr)
{
	ptr->ENTITY_STATE = ENTITY_STATE_DIE;
	ptr->ENTITY_ANIM = 100;
	snd_play(z_collect_snd, 100, 0);
	z_amount += Z_VALUE;
	ptr->DAMAGE_HIT = 0;
}

void Z__wait(ENTITY* ptr)
{
	ptr->pan += 5*time_step;
}


void Z__die(ENTITY* ptr)
{
	ptr->ENTITY_ANIM -= 15 * time_step;
	ptr->scale_x = 10*maxv(ptr->ENTITY_ANIM, 0) * 0.01;
	ptr->scale_y = 10*maxv(ptr->ENTITY_ANIM, 0) * 0.01;

	/* transitions */
	if(ptr->ENTITY_ANIM <= 0)
	{
		ptr->ENTITY_STATE = ENTITY_STATE_DEAD;
		set(ptr, PASSABLE);
		framework_remove(ptr);
	}
}
