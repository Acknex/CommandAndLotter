#include "global.h"
#include "framework.h"
#include "sputnik.h"
#include "scan.h"
#include "enemy_hit.h"
//#include "particle.h"
#include "ang.h"
#include "unit.h"


#define SPUTNIK_RUNSPEED skill1
#define SPUTNIK_TURNSPEED skill2
#define SPUTNIK_ATTACKSPEED skill3
#define SPUTNIK_ATTACKRANGE skill4
#define SPUTNIK_ACTIVEDIST skill10

#define SPUTNIK_ANIMSTATE skill21
#define SPUTNIK_ANIMSTATEATK skill22
#define SPUTNIK_STATE skill23
#define SPUTNIK_RUNSPEEDCUR skill24
#define SPUTNIK_SOUNDHANDLE skill25
#define SPUTNIK_DIDATTACK skill26
#define SPUTNIK_HITTHRESHOLD skill27


#define SPUTNIK_WALKANIM "walk"
#define SPUTNIK_WAITANIM "stand"
#define SPUTNIK_ATTACKANIM "AttackA"
#define SPUTNIK_DIEANIM "Die"


#define SPUTNIK_STATE_INACTIVE 0
#define SPUTNIK_STATE_WAIT 1
#define SPUTNIK_STATE_WALK 2
#define SPUTNIK_STATE_ATTACK 3
#define SPUTNIK_STATE_DIE 4
#define SPUTNIK_STATE_DEAD 5
#define SPUTNIK_STATE_HIT 6

#define SPUTNIK_FEET 30
#define SPUTNIK_TARGETDIST 100

SOUND* sputnik_snd_death = "sputnik_death.wav";
SOUND* sputnik_snd_attack1 = "sputnik_attack1.wav";
SOUND* sputnik_snd_attack2 = "sputnik_attack2.wav";
SOUND* sputnik_snd_attack3 = "sputnik_attack3.wav";


#define SPUTNIK_RUNSPEED skill1
#define SPUTNIK_TURNSPEED skill2
#define SPUTNIK_ATTACKSPEED skill3
#define SPUTNIK_ACTIVEDIST skill10
action Sputnik()
{
   framework_setup(my, SUBSYSTEM_UNIT_SPUTNIK);
	if(my->SPUTNIK_RUNSPEED == 0) my->SPUTNIK_RUNSPEED = 3;
	if(my->SPUTNIK_TURNSPEED == 0) my->SPUTNIK_TURNSPEED = 50;
	if(my->SPUTNIK_ATTACKSPEED == 0) my->SPUTNIK_ATTACKSPEED = 5;
	if(my->SPUTNIK_ATTACKRANGE == 0) my->SPUTNIK_ATTACKRANGE = 300;
	if(my->SPUTNIK_ACTIVEDIST == 0) my->SPUTNIK_ACTIVEDIST = 6000;
	my->SPUTNIK_ANIMSTATEATK = 100;
	my->HEALTH = 23;//HEALTH_SPUTNIK; TODO HOOK TO UNIT SYSTEM
	ENEMY_HIT_init(my);
	vec_scale(&my->scale_x, 1.2);
	set(my, SHADOW);
	c_setminmax(me);
	my->min_z += SPUTNIK_FEET;
	my->group = GROUP_UNIT;
	vec_set(my->UNIT_TARGET, my->x);
	my->SPUTNIK_STATE = SPUTNIK_STATE_WAIT;
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
			ptr->event = NULL;
			ptr->SPUTNIK_HITTHRESHOLD = 3;				
			ptr->push = -100;

			ptr->HEALTH = maxv(0, ptr->HEALTH - ptr->DAMAGE_HIT);
			ptr->DAMAGE_HIT = 0;

			if (ptr->HEALTH <= 0)
			{
				ptr->SPUTNIK_STATE = SPUTNIK_STATE_DIE;
				ptr->SPUTNIK_ANIMSTATE = 0;
				snd_play(sputnik_snd_death, 100, 0);
			}
		}
		
		if (ptr->SPUTNIK_SOUNDHANDLE && (snd_playing(ptr->SPUTNIK_SOUNDHANDLE) == 0))
		{
			snd_remove(ptr->SPUTNIK_SOUNDHANDLE);
			ptr->SPUTNIK_SOUNDHANDLE = 0;
		}
					
		switch(ptr->SPUTNIK_STATE)    	
		{

			case SPUTNIK_STATE_WAIT:
			{
				SPUTNIK__wait(ptr);
				break;
			}

			case SPUTNIK_STATE_WALK:
			{
				SPUTNIK__walk(ptr);
				break;
			}

			case SPUTNIK_STATE_DIE:
			{
				SPUTNIK__die(ptr);
				break;
			}

			default:
			{
				break;
			}
		}	
	
		if (ptr->SPUTNIK_STATE != SPUTNIK_STATE_DIE && ptr->SPUTNIK_STATE != SPUTNIK_STATE_DEAD)
		{
			VECTOR* from = vector(ptr->x, ptr->y, ptr->z);// + 100);
			VECTOR* to = vector(ptr->x, ptr->y, ptr->z - 1000);
			me = ptr;
			var mode = IGNORE_ME | IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_PUSH | IGNORE_SPRITES | IGNORE_CONTENT | USE_BOX;
			c_ignore(GROUP_UNIT,0);
			c_trace(from, to, mode);
			if (HIT_TARGET)
			{
				var newZ = hit.z - ptr->min_z + SPUTNIK_FEET;
				if (ptr->z < newZ)
					ptr->z = minv(ptr->z + 17* time_step, newZ);
				else
					ptr->z = maxv(ptr->z - 17* time_step, newZ);
			}
		}
		
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
		ptr->SPUTNIK_RUNSPEEDCUR = 0;
		ptr->SPUTNIK_STATE = SPUTNIK_STATE_WALK;
	}
}

void SPUTNIK__walk(ENTITY* ptr)
{
	//DEBUG_VAR(ptr->pan, 20);
	//DEBUG_VAR(ptr->UNIT_TARGETX, 120);
	//DEBUG_VAR(ptr->UNIT_TARGETY, 140);
	ptr->SPUTNIK_RUNSPEEDCUR = ptr->SPUTNIK_RUNSPEED; 
	ANG_turnToPos(ptr, ptr->UNIT_TARGET, ptr->SPUTNIK_TURNSPEED, 5);
	VECTOR* to = vector(ptr->SPUTNIK_RUNSPEEDCUR + 20, 0, 0);
	vec_rotate(to, vector(ptr->pan,0,0));
	vec_add (to, &ptr->x);
	var tracemode = IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_SPRITES | IGNORE_CONTENT;
	c_ignore (GROUP_UNIT);
	if (c_trace(&ptr->x, to, tracemode) <= 0)
	{
		var mode = IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_SPRITES | GLIDE;
		c_move(ptr, vector(ptr->SPUTNIK_RUNSPEEDCUR, 0, 0), nullvector, mode);
	}
		
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
		ptr->SPUTNIK_STATE = SPUTNIK_STATE_WAIT;
	}
	else // Still walking
	{
		ptr->SPUTNIK_DIDATTACK = 0;
		ptr->SPUTNIK_ANIMSTATEATK = 0;
		ptr->SPUTNIK_ANIMSTATE += ptr->SPUTNIK_RUNSPEED * time_step;
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
		ptr->SPUTNIK_STATE = SPUTNIK_STATE_DEAD;
		set(ptr, PASSABLE);
	}
}
