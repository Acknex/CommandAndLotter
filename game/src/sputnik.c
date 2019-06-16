#include "global.h"
#include "framework.h"
#include "sputnik.h"
#include "scan.h"
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
#define SPUTNIK_RUNCOUNTER skill27


#define SPUTNIK_WALKANIM "walk"
#define SPUTNIK_WAITANIM "stand"
#define SPUTNIK_ATTACKANIM "AttackA"
#define SPUTNIK_DIEANIM "Die"

#define SPUTNIK_FEET 30
#define SPUTNIK_TARGETDIST 70

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
	my->SPUTNIK_ATTACKRANGE = 170;
	my->SPUTNIK_ANIMSTATEATK = 0;
	my->HEALTH = 23;
	my->MAXHEALTH = my->HEALTH;
	set(my, SHADOW);
	c_setminmax(me);
	my->min_z += SPUTNIK_FEET;
	my->ENTITY_STATE = ENTITY_STATE_WAIT_OR_WALK;
	my->ENTITY_DAMAGE = 5;
}

void SPUTNIK_Init()
{
}


void SPUTNIK__wait_or_walk(ENTITY * ptr)
{
	UNIT* unit = jpsAllowMovementForEntity(ptr, true);
	if(!unit) return;
	
	vec_set(ptr->x, unit->pos3d);
	VECTOR diff, temp;
	vec_diff(diff, unit->pos3d, unit->prevPos3d);
	var len = vec_to_angle(temp, diff)/time_step;
	if(len > 8) ptr->SPUTNIK_RUNCOUNTER = 4;
	//if(unit->isMoving) ptr->SPUTNIK_RUNCOUNTER = 12;
	if(ptr->SPUTNIK_RUNCOUNTER > 0)
	{
		ptr->SPUTNIK_RUNCOUNTER = maxv(ptr->SPUTNIK_RUNCOUNTER-time_step,0);
		if(len > 8) ptr->pan += ang(temp.x-ptr->pan)*0.5*time_step;

		ptr->SPUTNIK_DIDATTACK = 0;
		ptr->SPUTNIK_ANIMSTATEATK = 0;
		ptr->SPUTNIK_ANIMSTATE += len*0.425*time_step;//0.5 * ptr->SPUTNIK_RUNSPEED * time_step;
		ent_animate(ptr, SPUTNIK_WALKANIM, ptr->SPUTNIK_ANIMSTATE, ANM_CYCLE);		
	}
	else
	{
		ptr->SPUTNIK_ANIMSTATE += 7 * time_step;
		ent_animate(ptr, SPUTNIK_WAITANIM, ptr->SPUTNIK_ANIMSTATE, ANM_CYCLE);
	}
	
	if (ptr->DAMAGE_HIT > 0)
	{
		if (ptr->ENTITY_STATE != ENTITY_STATE_HIT)
		{
			ptr->HEALTH = maxv(0, ptr->HEALTH - ptr->DAMAGE_HIT);
			ptr->ENTITY_STATE = ENTITY_STATE_HIT;
			ptr->ENTITY_HITTHRESHOLD = 5;				
		}
		ptr->DAMAGE_HIT = 0;
	}
	else if (SCAN_IsEntityNear(ptr, unit_getVictim(ptr), ptr->SPUTNIK_ATTACKRANGE))
	{
		ptr->ENTITY_STATE = ENTITY_STATE_ATTACK;		
	}
}


void SPUTNIK_Update()
{
	ENTITY * ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_UNIT_SPUTNIK)
	{
		jpsAllowMovementForEntity(ptr, false);
		
		switch(ptr->ENTITY_STATE)    	
		{
			case ENTITY_STATE_WAIT_OR_WALK:
			{
				SPUTNIK__wait_or_walk(ptr);
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
			
			case ENTITY_STATE_HIT:
			{
				SPUTNIK__hit(ptr);
				break;
			}
			
			default:
			{
				break;
			}
		}	
		
		if (ptr->ENTITY_STATE != ENTITY_STATE_DIE && ptr->ENTITY_STATE != ENTITY_STATE_DEAD)
		{
			UNIT* unit = jpsUnitGetFromEntity(ptr);
			if(unit)
			{
				TILE* tile = unit->tile;
				if(tile)
				{
					if(!tile->value) ptr->z = maploader_get_height(ptr->x) - ptr->min_z + SPUTNIK_FEET;
				}	
			}
		}
	}	
}

void SPUTNIK__findNextVictim(ENTITY* ptr)
{
	if (unit_getVictim(ptr) == NULL)
	{
		//meh.
		var owner;
		if (ptr->group == GROUP_ENEMY_UNIT)	
		owner = UNIT_ENEMY;
		else
		owner = UNIT_PLAYER;
		
		cprintf0("\n SPUTNIK__findNextVictim: TRYFIND...");
		
		if(ptr->ENTITY_VICTIMTYPE == UNIT_Z)
		{
			ENTITY *zUnit, *zUnitNear = NULL;
			var minDist = 1000;
			SUBSYSTEM_LOOP(zUnit, SUBSYSTEM_Z)
			{
				VECTOR zPos;
				vec_set(&zPos, &zUnit->x);
				vec_sub(&zPos, &ptr->x);
				if(vec_length(&zPos) < minDist)
				{
					zUnitNear = zUnit;
					minDist = vec_length(&zPos);
				}
			}
			if(zUnitNear)
			{
				unit_setTarget(ptr, zUnitNear->x);
				unit_setVictim(ptr, zUnitNear);
			}
		}
		else
		{
			int count = mapGetNearbyUnitsOfTypeForPos(ptr->x, ptr->ENTITY_VICTIMTYPE, owner, 1000, 1);
			if (count > 0)
			{
				cprintf1("FOUND!(%d)", count);
				ENTITY* ent = jpsGetEntityFromUnitArray(0);
				
				//set new target and victim
				unit_setTarget(ptr, ent->x);
				unit_setVictim(ptr, ent);
			}
		}
	}
}

void SPUTNIK__hit(ENTITY* ptr)
{
	ptr->ENTITY_HITTHRESHOLD -= time_step;		
	if (ptr->HEALTH <= 0)
	{
		ptr->ENTITY_STATE = ENTITY_STATE_DIE;
		ptr->SPUTNIK_ANIMSTATE = 0;
		snd_play(sputnik_snd_death, 100, 0);
		set(ptr, PASSABLE);		
	}
	else if (ptr->ENTITY_HITTHRESHOLD <= 0)
	{
		ptr->ENTITY_STATE = ENTITY_STATE_WAIT_OR_WALK;
	}
	else
	{
		var percent = (1 - (ptr->ENTITY_HITTHRESHOLD/5)) * 56;
		if (percent > 28)
		percent = 56-percent;
		ent_animate(ptr, SPUTNIK_ATTACKANIM, percent, 0);
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
			ENTITY* victim = unit_getVictim(ptr);
			if (SCAN_IsEntityNear(ptr, victim, ptr->SPUTNIK_ATTACKRANGE))
			{
				unit_setDamage(victim, ptr->ENTITY_DAMAGE);
			}			
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
		SPUTNIK__findNextVictim(ptr);
		ptr->ENTITY_STATE = ENTITY_STATE_WAIT_OR_WALK;
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
	}
}
