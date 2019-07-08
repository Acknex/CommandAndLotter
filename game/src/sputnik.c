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
#define SPUTNIK_IDLECOUNTER skill28


#define SPUTNIK_WALKANIM "walk"
#define SPUTNIK_WAITANIM "stand"
#define SPUTNIK_ATTACKANIM "AttackA"
#define SPUTNIK_DIEANIM "Die"

#define SPUTNIK_FEET 30
#define SPUTNIK_MAXIDLE 32

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
	my->SPUTNIK_ATTACKRANGE = 200;
	my->SPUTNIK_ANIMSTATEATK = 0;
	my->SPUTNIK_IDLECOUNTER = 0;
	reset(my, SHADOW);
	c_setminmax(me);
	my->min_z += SPUTNIK_FEET;
	my->ENTITY_STATE = ENTITY_STATE_WAIT_OR_WALK;
}

void SPUTNIK_Init()
{
}

void SPUTNIK__wait_or_walk_babe(ENTITY * ptr)
{
	UNIT* unit = jpsAllowForEntity(ptr, true);
	if(!unit) return;
	ptr->x = unit->pos3d.x;
	ptr->y = unit->pos3d.y;
	VECTOR diff, temp;
	vec_diff(diff, unit->currentTarget3d, unit->prevPos3d);

	int decreaseSpeedTrue = 1;
	var len = vec_to_angle(temp, diff)/time_step;
	var angDiff = ang(temp.x-ptr->pan);
	if(len > 64)
	{
		ptr->pan += clamp(angDiff*0.25,-12,12)*time_step;
		var turnSpeed = 5+unit->speedFac*10;
		if(abs(angDiff) < 15)
		{
			unit->speedFac = minv(unit->speedFac+0.1*time_step,1);
			decreaseSpeedTrue = 0;
		}
	}
	if(decreaseSpeedTrue)
	{
		unit->speedFac = maxv(unit->speedFac-0.15*time_step,0.5);
		vec_set(unit->speedOverwrite, vector(1,0,0));
		vec_rotate(unit->speedOverwrite, vector(ptr->pan,0,0));
		unit->speedOverwrite.z = clamp((abs(angDiff)-15)/15.0,0,1)*clamp((len-8)/8.0,0,1);
	}
	else unit->speedOverwrite.z = 0;
	jpsAllowMovementForEntity(ptr, true);
}


void SPUTNIK__wait_or_walk(ENTITY * ptr)
{
	if(ptr->ENTITY_UNITTYPE == UNIT_BABE)
	{
		SPUTNIK__wait_or_walk_babe(ptr);
		return;
	}
	UNIT* unit = jpsAllowForEntity(ptr, true);
	if(!unit) return;

	ptr->x = unit->pos3d.x;
	ptr->y = unit->pos3d.y;
	VECTOR diff, temp;
	VECTOR* pvTarget = &unit->pos3d;
	if(unit->attackCooldown > 0)
	{
		if(unit->victimUnit) pvTarget = &unit->victimUnit->pos3d;
	}
	vec_diff(diff, pvTarget, unit->prevPos3d);

	var len = vec_to_angle(temp, diff)/time_step;

	if(len > 8) ptr->SPUTNIK_RUNCOUNTER = 4;
	//if(unit->isMoving) ptr->SPUTNIK_RUNCOUNTER = 12;
	if(len > 8 || unit->attackCooldown > 0) ptr->pan += ang(temp.x-ptr->pan)*0.5*time_step;
	
	if(unit->attackCooldown > 0)
	{
		//draw_line3D2(vector(ptr->x,ptr->y,ptr->z+64), vector(unit->target3d.x,unit->target3d.y,unit->target3d.z+64), COLOR_RED,20);
		/*VECTOR temp;
		vec_set(temp, ptr->x);
		if(vec_to_screen(temp,camera))
		{
			STRING* str = str_printf(NULL, "%.1f", (double)unit->attackCooldown);
			draw_text(str, temp.x-9,temp.y-9,COLOR_BLACK);
			draw_text(str, temp.x-10,temp.y-10,COLOR_WHITE);
		}*/
		if (ptr->SPUTNIK_ANIMSTATEATK == 0)
		{
			if (num_sounds < 8)
			{
				switch(integer(random(3)))
				{
					case 0: ent_playsound(ptr,sputnik_snd_attack1, 7000); break;
					case 1: ent_playsound(ptr,sputnik_snd_attack2, 7000); break;
					case 2: ent_playsound(ptr,sputnik_snd_attack3, 7000); break;
				}
			}
		}
		ptr->SPUTNIK_ANIMSTATEATK += ptr->SPUTNIK_ATTACKSPEED * time_step;
		ptr->SPUTNIK_ANIMSTATEATK = minv(ptr->SPUTNIK_ANIMSTATEATK, 100);
		if(!(ptr->eflags & CLIPPED)) ent_animate(ptr, SPUTNIK_ATTACKANIM, ptr->SPUTNIK_ANIMSTATEATK, 0);
	}
	else
	{
		ptr->SPUTNIK_ANIMSTATEATK = 0;
		if(ptr->SPUTNIK_RUNCOUNTER > 0)
		{
			ptr->SPUTNIK_RUNCOUNTER = maxv(ptr->SPUTNIK_RUNCOUNTER-time_step,0);

			ptr->SPUTNIK_DIDATTACK = 0;
			ptr->SPUTNIK_ANIMSTATEATK = 0;
			ptr->SPUTNIK_ANIMSTATE += len*0.65*time_step;//0.5 * ptr->SPUTNIK_RUNSPEED * time_step;
			if(!(ptr->eflags & CLIPPED)) ent_animate(ptr, SPUTNIK_WALKANIM, ptr->SPUTNIK_ANIMSTATE, ANM_CYCLE);

			ptr->SPUTNIK_IDLECOUNTER = 0;
		}
		else
		{
			ptr->SPUTNIK_IDLECOUNTER += time_step;
			ptr->SPUTNIK_ANIMSTATE += 7 * time_step;
			if(!(ptr->eflags & CLIPPED)) ent_animate(ptr, SPUTNIK_WAITANIM, ptr->SPUTNIK_ANIMSTATE, ANM_CYCLE);
		}
		
		//if(unit->attackMove) draw_line3D2(vector(ptr->x,ptr->y,ptr->z+64), vector(unit->target3d.x,unit->target3d.y,unit->target3d.z+64), vector(0,255,255),20);
		//else draw_line3D2(vector(ptr->x,ptr->y,ptr->z+64), vector(unit->target3d.x,unit->target3d.y,unit->target3d.z+64), COLOR_GREEN,20);
	}
}


void SPUTNIK_Update()
{
	ENTITY * ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_UNIT_SPUTNIK)
	{
		if(ptr->ENTITY_STATE != ENTITY_STATE_WAIT_OR_WALK) ptr->SPUTNIK_IDLECOUNTER = 0;

		UNIT* unit = jpsAllowForEntity(ptr, false);

		/*switch(ptr->ENTITY_STATE)
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
		}*/
		
		if(unit)
		{
			ptr->ENTITY_STATE = ENTITY_STATE_WAIT_OR_WALK;
			SPUTNIK__wait_or_walk(ptr);
		}
		else
		{
			
			if(ptr->ENTITY_STATE != ENTITY_STATE_DIE && ptr->ENTITY_STATE != ENTITY_STATE_DEAD) ptr->SPUTNIK_ANIMSTATE = 0;
			ptr->ENTITY_STATE = ENTITY_STATE_DIE;
			SPUTNIK__die(ptr);
		}

		if (ptr->ENTITY_STATE != ENTITY_STATE_DIE && ptr->ENTITY_STATE != ENTITY_STATE_DEAD)
		{
			UNIT* unit = jpsUnitGetFromEntity(ptr);
			if(unit)
			{
				TILE* tile = unit->tile;
				if(tile)
				{
					if(!tile->value)
					{
						var targetZ = maploader_get_height(ptr->x) - ptr->min_z - SPUTNIK_FEET;
						ptr->z += (targetZ-ptr->z)*0.5*time_step;
					}
				}
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
		ent_playsound(ptr,sputnik_snd_death, 10000);
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
		if (num_sounds < 6)
		{
			switch(integer(random(2)))
			{
				case 0: ent_playsound(ptr,sputnik_snd_attack1, 7000); break;
				case 1: ent_playsound(ptr,sputnik_snd_attack2, 7000); break;
				case 2: ent_playsound(ptr,sputnik_snd_attack3, 7000); break;
			}
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
				//DEBUG
				if (victim == ptr) error ("attacking myself :O");
				unit_setDamage(victim, ptr);
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
		//unit_findNextVictim(ptr);
		ptr->SPUTNIK_ANIMSTATEATK = 0;
		ptr->ENTITY_STATE = ENTITY_STATE_WAIT_OR_WALK;
	}

}

void SPUTNIK__die(ENTITY* ptr)
{
	if(ptr->ENTITY_UNITTYPE == UNIT_SPUTNIK)
	{
	ptr->SPUTNIK_ANIMSTATE += 5 * time_step;
	ent_animate(ptr, SPUTNIK_DIEANIM, ptr->SPUTNIK_ANIMSTATE, 0);
}
	else
	{
		ptr->roll = 180;
	}
	/* transitions */
		unit_deactivate(ptr);
	if(ptr->SPUTNIK_ANIMSTATE >= 90)
	{
		ptr->ENTITY_STATE = ENTITY_STATE_DEAD;
		framework_remove(ptr);
	}
}
