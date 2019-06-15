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
#define SPUTNIK_DIDATTACK skill26
#define SPUTNIK_HITTHRESHOLD skill27


#define SPUTNIK_WALKANIM "walk"
#define SPUTNIK_WAITANIM "stand"
#define SPUTNIK_ATTACKANIM "AttackA"
#define SPUTNIK_DIEANIM "Die"

#define SPUTNIK_FEET 30
#define SPUTNIK_TARGETDIST 100

SOUND* sputnik_snd_death   = "sputnik_death.wav";
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
	my->SPUTNIK_ANIMSTATEATK = 0;
	my->HEALTH = 23;
	my->MAXHEALTH = my->HEALTH;
	ENEMY_HIT_init(my);
	set(my, SHADOW);
	c_setminmax(me);
	my->min_z += SPUTNIK_FEET;
	my->ENTITY_STATE = ENTITY_STATE_WAIT;
	my->ENTITY_DAMAGE = 5;
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

			case ENTITY_STATE_ATTACK:
			{
				SPUTNIK__attack(ptr);
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
	DEBUG_VAR(ptr->x, 120);
	DEBUG_VAR(ptr->y, 140);
		
	DEBUG_VAR(ptr->UNIT_TARGETX, 180);
	DEBUG_VAR(ptr->UNIT_TARGETY, 200);
	}	
}

void SPUTNIK__hit(ENTITY* ptr)
{
	ptr->event = NULL;
	ptr->SPUTNIK_HITTHRESHOLD = 3;				

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
		}
	}
}

void SPUTNIK__wait(ENTITY* ptr)
{
  	ptr->SPUTNIK_ANIMSTATE += 7 * time_step;
	ent_animate(ptr, SPUTNIK_WAITANIM, ptr->SPUTNIK_ANIMSTATE, ANM_CYCLE);
		
	/* transitions */
	// Unit is near enough to attack
	if (SCAN_IsEntityNear(ptr, unit_getVictim(ptr), ptr->SPUTNIK_ATTACKRANGE))
	{
		ptr->ENTITY_STATE = ENTITY_STATE_ATTACK;
	}
	else if (SCAN_IsTargetNear(ptr, unit_getTarget(ptr), SPUTNIK_TARGETDIST) == 0)
	{
		ptr->ENTITY_STATE = ENTITY_STATE_WALK;
	}
	else
	{
		//keep waiting
	}
}

void SPUTNIK__walk(ENTITY* ptr)
{
	ANG_turnToPos(ptr, unit_getTarget(ptr), ptr->SPUTNIK_TURNSPEED, 5);
	VECTOR moveVec;

	vec_set(&moveVec, vector(ptr->SPUTNIK_RUNSPEED * time_step, 0, 0));
	vec_rotate(&moveVec, vector(ptr->pan, 0, 0));
	vec_add(&ptr->x, moveVec);
	
	/* transitions */
	// Unit reached target
	if (SCAN_IsTargetNear(ptr, unit_getTarget(ptr), SPUTNIK_TARGETDIST))
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

void SPUTNIK__attack(ENTITY* ptr)
{
	ANG_turnToPos(ptr, unit_getTarget(ptr), ptr->SPUTNIK_TURNSPEED, 5);
	if (ptr->SPUTNIK_ANIMSTATEATK == 0)
	{
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
			me = ptr;
			var mode = IGNORE_ME | IGNORE_WORLD | IGNORE_PUSH | IGNORE_SPRITES | IGNORE_CONTENT | ACTIVATE_SHOOT;
			if (ptr->group == GROUP_PLAYER_UNIT)
				c_ignore(GROUP_PLAYER_UNIT, GROUP_PLAYER_SPAWNER,0);
			else
				c_ignore(GROUP_ENEMY_UNIT, GROUP_ENEMY_SPAWNER,0);
			//ENEMY_HIT_setAttack()
			c_trace(ptr->x, unit_getTarget(ptr), mode);			
		}
		ptr->SPUTNIK_DIDATTACK = 1;
	}
	else
	{
		ptr->SPUTNIK_DIDATTACK = 0;
	}

	if (ptr->SPUTNIK_ANIMSTATEATK >= 100)
	{
		ptr->SPUTNIK_ANIMSTATEATK = 0;
		ptr->ENTITY_STATE = ENTITY_STATE_WAIT;
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
