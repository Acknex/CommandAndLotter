#include "global.h"
#include "framework.h"
#include "sputnik.h"
#include "scan.h"
#include "enemy_hit.h"
#include "ang.h"
#include "unit.h"
#include "map_loader.h"


#define SPUTNIK_RUNSPEED skill1
#define SPUTNIK_TURNSPEED skill2
#define SPUTNIK_ATTACKSPEED skill3
#define SPUTNIK_ATTACKRANGE skill4

#define SPUTNIK_ANIMSTATE skill21
#define SPUTNIK_ANIMSTATEATK skill22
#define SPUTNIK_SOUNDHANDLE skill25
#define SPUTNIK_DIDATTACK skill26
#define SPUTNIK_HITTHRESHOLD skill27


#define SPUTNIK_WALKANIM "walk"
#define SPUTNIK_WAITANIM "stand"
#define SPUTNIK_ATTACKANIM "AttackA"
#define SPUTNIK_DIEANIM "Die"


#define ENTITY_STATE_INACTIVE 0
#define ENTITY_STATE_WAIT 1
#define ENTITY_STATE_WALK 2
#define ENTITY_STATE_ATTACK 3
#define ENTITY_STATE_DIE 4
#define ENTITY_STATE_DEAD 5
#define ENTITY_STATE_HIT 6

#define SPUTNIK_FEET 30
#define SPUTNIK_TARGETDIST 100

SOUND* sputnik_snd_death = "sputnik_death.wav";
SOUND* sputnik_snd_attack1 = "sputnik_attack1.wav";
SOUND* sputnik_snd_attack2 = "sputnik_attack2.wav";
SOUND* sputnik_snd_attack3 = "sputnik_attack3.wav";


void Sputnik()
{
   framework_setup(my, SUBSYSTEM_UNIT_SPUTNIK);
	my->SPUTNIK_RUNSPEED = 30;
	my->SPUTNIK_TURNSPEED = 50;
	my->SPUTNIK_ATTACKSPEED = 5;
	my->SPUTNIK_ATTACKRANGE = 300;
	my->SPUTNIK_ANIMSTATEATK = 100;
	my->HEALTH = 23;//HEALTH_SPUTNIK; TODO HOOK TO UNIT SYSTEM
	ENEMY_HIT_init(my);
	vec_scale(&my->scale_x, 1.2);
	set(my, SHADOW);
	c_setminmax(me);
	my->min_z += SPUTNIK_FEET;
	my->group = GROUP_UNIT;
	vec_set(my->UNIT_TARGET, my->x);
	my->ENTITY_STATE = ENTITY_STATE_WAIT;
}

void SPUTNIK_Init()
{
}

void SPUTNIK_Update()
{
	ENTITY * ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_UNIT_SPUTNIK)
	{
			
		if (ptr->HEALTH > 0)
		{
			SPUTNIK__hitcheck(ptr);
		}
		
		if (ptr->DAMAGE_HIT > 0)
      {
      	SPUTNIK__hit(ptr);
		}
		
		if (ptr->SPUTNIK_SOUNDHANDLE && (snd_playing(ptr->SPUTNIK_SOUNDHANDLE) == 0))
		{
			snd_remove(ptr->SPUTNIK_SOUNDHANDLE);
			ptr->SPUTNIK_SOUNDHANDLE = 0;
		}
					
		switch(ptr->ENTITY_STATE)    	
		{

			case ENTITY_STATE_WAIT:
			{
				SPUTNIK__wait(ptr);
				break;
			}

			case ENTITY_STATE_WALK:
			{
				SPUTNIK__walk(ptr);
				break;
			}

			case ENTITY_STATE_DIE:
			{
				SPUTNIK__die(ptr);
				break;
			}

			default:
			{
				break;
			}
		}	
	
		if (ptr->ENTITY_STATE != ENTITY_STATE_DIE && ptr->ENTITY_STATE != ENTITY_STATE_DEAD)
		{
			ptr->z = maploader_get_height(ptr->x) - ptr->min_z + SPUTNIK_FEET;			
		}
		
	}	
}

void SPUTNIK__hit(ENTITY* ptr)
{
	ptr->event = NULL;
	ptr->SPUTNIK_HITTHRESHOLD = 3;				
	ptr->push = -100;

	ptr->HEALTH = maxv(0, ptr->HEALTH - ptr->DAMAGE_HIT);
	ptr->DAMAGE_HIT = 0;

	if (ptr->HEALTH <= 0)
	{
		ptr->ENTITY_STATE = ENTITY_STATE_DIE;
		ptr->SPUTNIK_ANIMSTATE = 0;
		snd_play(sputnik_snd_death, 100, 0);
	}
}

void SPUTNIK__hitcheck(ENTITY* ptr)
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

void SPUTNIK__wait(ENTITY* ptr)
{
  	ptr->SPUTNIK_ANIMSTATE += 10 * time_step;
	ent_animate(ptr, SPUTNIK_WAITANIM, ptr->SPUTNIK_ANIMSTATE, ANM_CYCLE);
		
	/* transitions */
	if (SCAN_IsTargetNear(ptr, ptr->UNIT_TARGET, SPUTNIK_TARGETDIST) == 0)
	{
		ptr->ENTITY_STATE = ENTITY_STATE_WALK;
	}
}

void SPUTNIK__walk(ENTITY* ptr)
{
	ANG_turnToPos(ptr, ptr->UNIT_TARGET, ptr->SPUTNIK_TURNSPEED, 5);
	VECTOR moveVec;

	vec_set(&moveVec, vector(ptr->SPUTNIK_RUNSPEED * time_step, 0, 0));
	vec_rotate(&moveVec, vector(ptr->pan, 0, 0));
	vec_add(&ptr->x, moveVec);
	
	
#ifdef Adjust_attack_code_to_new_project
	// Unit is near enough to attack
	if (SCAN_IsUnitnSight(ptr, ptr->SPUTNIK_ATTACKRANGE, 90))
	{
		if ((ptr->SPUTNIK_ANIMSTATEATK == 0) || (ptr->SPUTNIK_ANIMSTATEATK >= 100))
		{
			ptr->SPUTNIK_ANIMSTATEATK = 0;
			switch(integer(random(2)))
			{
				case 0: snd_play(sputnik_snd_attack1, 100, 0); break;
				case 1: snd_play(sputnik_snd_attack2, 100, 0); break;
				case 2: snd_play(sputnik_snd_attack3, 100, 0); break;
			}
		}
		ptr->SPUTNIK_ANIMSTATEATK += ptr->SPUTNIK_ATTACKSPEED * time_step;
		ptr->SPUTNIK_ANIMSTATEATK = minv(ptr->SPUTNIK_ANIMSTATEATK, 100);
		ptr->SPUTNIK_ANIMSTATE = 0;
		ent_animate(ptr, SPUTNIK_ATTACKANIM, ptr->SPUTNIK_ANIMSTATEATK, 0);
		
		if (ptr->SPUTNIK_ANIMSTATEATK > 50)
		{
			if (ptr->SPUTNIK_DIDATTACK == 0)
			{
				playerAddHealth(-DAMAGE_SPUTNIK);
			}
			ptr->SPUTNIK_DIDATTACK = 1;
		}
		else
		{
			ptr->SPUTNIK_DIDATTACK = 0;
		}
	}
	else 
#endif
	
	if (SCAN_IsTargetNear(ptr, ptr->UNIT_TARGET, SPUTNIK_TARGETDIST))
	{
		ptr->ENTITY_STATE = ENTITY_STATE_WAIT;
	}
	else // Still walking
	{
		ptr->SPUTNIK_DIDATTACK = 0;
		ptr->SPUTNIK_ANIMSTATEATK = 0;
		ptr->SPUTNIK_ANIMSTATE += 0.5 * ptr->SPUTNIK_RUNSPEED * time_step;
		ent_animate(ptr, SPUTNIK_WALKANIM, ptr->SPUTNIK_ANIMSTATE, ANM_CYCLE);		
	}
}

void SPUTNIK__die(ENTITY* ptr)
{
	ptr->SPUTNIK_ANIMSTATE += 5 * time_step;
	ent_animate(ptr, SPUTNIK_DIEANIM, ptr->SPUTNIK_ANIMSTATE, 0);
	/* transitions */
	if(ptr->SPUTNIK_ANIMSTATE >= 90)
	{
		ptr->ENTITY_STATE = ENTITY_STATE_DEAD;
		set(ptr, PASSABLE);
	}
}
