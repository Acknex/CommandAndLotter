#include "global.h"
#include "framework.h"
#include "spawner.h"
#include "enemy_hit.h"
#include "particle.h"
#include "map_loader.h"

#define SPAWNER_QUEUE skill21
#define SPAWNER_PROGRESS skill22
#define SPAWNER_BUILDTIMER skill23
#define SPAWNER_DIETIMER skill24
#define SPAWNER_HITTHRESHOLD skill27


#define SPAWNER_ACTIVEANIM "stand"
#define SPAWNER_PRODUCEANIM "Produce"
#define SPAWNER_DIEANIM "Die"


#define SPAWNER_STATE_CONSTRUCT 0
#define SPAWNER_STATE_ACTIVE 1
#define SPAWNER_STATE_PRODUCE 2
#define SPAWNER_STATE_DIE 4
#define SPAWNER_STATE_DEAD 5
#define SPAWNER_STATE_HIT 6

#define SPAWNER_BUILDTIME 20
#define SPAWNER_DIETIME 30

void Spawner()
{
   framework_setup(my, SUBSYSTEM_SPAWNER);
	my->HEALTH = 23;//TODO HOOK TO UNIT SYSTEM
	ENEMY_HIT_init(my);
	vec_scale(&my->scale_x, 1.2);
	set(my, SHADOW);
	c_setminmax(me);
	my->ENTITY_STATE = SPAWNER_STATE_CONSTRUCT;

	my->SPAWNER_QUEUE = 0;
	my->SPAWNER_BUILDTIMER = SPAWNER_BUILDTIME;
	my->SPAWNER_DIETIMER = SPAWNER_DIETIME;

	my->z = maploader_get_height(my->x) - my->min_z;			
	
}

void SPAWNER_Init()
{
}

void SPAWNER_Update()
{
	ENTITY * ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_SPAWNER)
	{

		if (ptr->HEALTH > 0)
		{
			SPAWNER__hitcheck(ptr);
		}
	
		if (ptr->DAMAGE_HIT > 0)
      {
      	SPAWNER__hit(ptr);
		}
	

		switch(ptr->ENTITY_STATE)    	
		{

			case SPAWNER_STATE_CONSTRUCT:
			{
				SPAWNER__construct(ptr);
				break;
			}

			case SPAWNER_STATE_ACTIVE:
			{
				SPAWNER__active(ptr);
				break;
			}

			case SPAWNER_STATE_PRODUCE:
			{
				SPAWNER__produce(ptr);
				break;
			}

			case SPAWNER_STATE_DIE:
			{
				SPAWNER__die(ptr);
				break;
			}

			default:
			{
				break;
			}
		}	
	
	}	
}

void SPAWNER__hit(ENTITY* ptr)
{
	ptr->event = NULL;
	ptr->SPUTNIK_HITTHRESHOLD = 3;				
	ptr->push = -100;

	ptr->HEALTH = maxv(0, ptr->HEALTH - ptr->DAMAGE_HIT);
	ptr->DAMAGE_HIT = 0;

	if (ptr->HEALTH <= 0)
	{
		ptr->ENTITY_STATE = SPAWNER_STATE_DIE;
		ptr->ENTITY_ANIM = 0;
		snd_play(sputnik_snd_death, 100, 0);
	}
}

void SPAWNER__hitcheck(ENTITY* ptr)
{
	if (ptr->SPUTNIK_HITTHRESHOLD > 0)
	{
		ptr->SPUTNIK_HITTHRESHOLD -= time_step;
		
		if (ptr->SPUTNIK_HITTHRESHOLD <= 0)
		{
			ptr->SPUTNIK_HITTHRESHOLD = 0;
			ptr->event = ENEMY_HIT_event;
			vec_zero(ptr->DAMAGE_VEC);
		}
	}
}


void SPAWNER__construct(ENTITY* ptr)
{
  	ptr->SPAWNER_PROGRESS += 10 * time_step;
  	//TODO: update shader
	if (ptr->SPAWNER_PROGRESS >= 100)
	{
		ptr->SPAWNER_PROGRESS = 100;
		ptr->ENTITY_STATE = SPAWNER_STATE_ACTIVE;
	}

}

void SPAWNER__active(ENTITY* ptr)
{
	if (ptr->SPAWNER_QUEUE > 0)
	{
		ptr->ENTITY_STATE = SPAWNER_STATE_PRODUCE;
	}
	else
	{
		//idle
	}
}

void SPAWNER__produce(ENTITY* ptr)
{
	ptr->SPAWNER_BUILDTIMER -= time_step;
	
	if (ptr->SPAWNER_BUILDTIMER <= 0)
	{
		ptr->SPAWNER_BUILDTIMER = SPAWNER_BUILDTIME;
		ptr->SPAWNER_QUEUE--;
		//TODO spawn unit
	}
	if (ptr->SPAWNER_QUEUE == 0)
	{
		ptr->SPAWNER_BUILDTIMER = SPAWNER_BUILDTIME;
		ptr->ENTITY_STATE = SPAWNER_STATE_ACTIVE;
	}
}

void SPAWNER__die(ENTITY* ptr)
{
	ptr->SPAWNER_DIETIMER -= time_step;

	VECTOR* pos = vector(ptr->x+random(10)-5, ptr->y+random(10)-5, ptr->z+random(10)-5);
	VECTOR* vel = vector(-5-random(10), -2-random(4), 2+random(4));
	effect(PARTICLE_smoke, 6*time_step, pos, vel);

	/* transitions */
	if(ptr->SPAWNER_DIETIMER <= 0)
	{
		reset(ptr, SHADOW);
		ptr->SPAWNER_DIETIMER  = 0;
		ptr->ENTITY_STATE = SPAWNER_STATE_DEAD;
	}
}
