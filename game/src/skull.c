#include "global.h"
#include "framework.h"
#include "skull.h"
#include "scan.h"
#include "ang.h"
#include "unit.h"
#include "map_loader.h"


#define SKULL_TURNSPEED skill2
#define SKULL_ATTACKRANGE skill4
#define SKULL_ANIMSTATEATK skill22
#define SKULL_DIDATTACK skill26
#define SKULL_RUNCOUNTER skill27
#define SKULL_IDLECOUNTER skill28


#define SKULL_WALKANIM "stand"
#define SKULL_WAITANIM "stand"
#define SKULL_HITANIM "stand"

#define SKULL_FEET -20
#define SKULL_MAXIDLE 16

SOUND* skull_snd_death  = "skull_death.wav";
SOUND* skull_snd_attack = "skull_shoot.wav";
SOUND* skull_snd_hit1   = "skull_hit1.wav";
SOUND* skull_snd_hit2   = "skull_hit2.wav";

void Skull()
{
	framework_setup(my, SUBSYSTEM_UNIT_SKULL);
	my->SKULL_TURNSPEED = 50;
	my->SKULL_ATTACKRANGE = 200;
	my->SKULL_ANIMSTATEATK = 0;
	my->SKULL_IDLECOUNTER = 0;
	reset(my, SHADOW);
	c_setminmax(me);
	my->min_z += SKULL_FEET;
	my->ENTITY_STATE = ENTITY_STATE_WAIT_OR_WALK;
}

void SKULL_Init()
{
}


void SKULL__wait_or_walk(ENTITY * ptr)
{
	UNIT* unit = jpsAllowMovementForEntity(ptr, true);
	if(!unit) return;

	vec_set(ptr->x, unit->pos3d);
	VECTOR diff, temp;
	vec_diff(diff, unit->pos3d, unit->prevPos3d);

	var len = vec_to_angle(temp, diff)/time_step;

	if(len > 8) ptr->SKULL_RUNCOUNTER = 4;

	if(ptr->SKULL_RUNCOUNTER > 0)
	{
		ptr->SKULL_RUNCOUNTER = maxv(ptr->SKULL_RUNCOUNTER-time_step,0);
		if(len > 8) ptr->pan += ang(temp.x-ptr->pan)*0.5*time_step;

		ptr->SKULL_DIDATTACK = 0;
		ptr->SKULL_ANIMSTATEATK = 0;
		ptr->ENTITY_ANIM += len*0.425*time_step;
		ent_animate(ptr, SKULL_WALKANIM, ptr->ENTITY_ANIM, ANM_CYCLE);

		ptr->SKULL_IDLECOUNTER = 0;
	}
	else
	{
		ptr->SKULL_IDLECOUNTER += time_step;
		ptr->ENTITY_ANIM += 7 * time_step;
		ent_animate(ptr, SKULL_WAITANIM, ptr->ENTITY_ANIM, ANM_CYCLE);
	}

	//skull was hit?
	if (unit_checkHit(ptr))
	{
		ptr->ENTITY_STATE = ENTITY_STATE_HIT;
		ptr->ENTITY_HITTHRESHOLD = 5;
		switch(integer(random(1)))
		{
			case 0: ent_playsound(ptr,skull_snd_hit1, 7000); break;
			case 1: ent_playsound(ptr,skull_snd_hit2, 7000); break;
		}
	}	
	else
	{
		//selected victim is near - attack
		if (SCAN_IsEntityNear(ptr, unit_getVictim(ptr), ptr->SKULL_ATTACKRANGE))
		{
			ptr->ENTITY_STATE = ENTITY_STATE_ATTACK;
			ptr->ENTITY_ANIM = 0;
		}
		//nothing to do? go mining
		if(ptr->SKULL_IDLECOUNTER > SKULL_MAXIDLE && !unit_getVictim(ptr))
		{
			unit_findNextVictim(ptr, UNIT_Z);
		}
		
	}
}


void SKULL_Update()
{
	ENTITY * ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_UNIT_SKULL)
	{
		if(ptr->ENTITY_STATE != ENTITY_STATE_WAIT_OR_WALK)
			ptr->SKULL_IDLECOUNTER = 0;

		jpsAllowMovementForEntity(ptr, false);

		switch(ptr->ENTITY_STATE)
		{
			case ENTITY_STATE_WAIT_OR_WALK:
			{
				SKULL__wait_or_walk(ptr);
				break;
			}

			case ENTITY_STATE_ATTACK:
			{
				SKULL__attack(ptr);
				break;
			}

			case ENTITY_STATE_DIE:
			{
				SKULL__die(ptr);
				break;
			}

			case ENTITY_STATE_HIT:
			{
				SKULL__hit(ptr);
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
                    if(!tile->value) ptr->z = maploader_get_height(ptr->x) - ptr->min_z - SKULL_FEET;
				}
			}

			int vertices[] = {73, 200, 67, 201, 68};
			CONTACT contact;
			int i;
			for(i = 0; i < 5; i++)
			{
				ent_getvertex(ptr, &contact, vertices[i]);
				vec_scale(contact.x, ptr->scale_x);
				vec_rotate(contact.x, ptr->pan);
				vec_add(contact.x, ptr->x);
				
				VECTOR velocity;
				vec_set(velocity, nullvector);
				velocity.x = -20 - random(20);
				velocity.z = 20 + random(30);
				vec_rotate(velocity, ptr->pan);
				if (ptr->ENTITY_STATE == ENTITY_STATE_HIT)
				{
					effect(SKULL__smokeEffect, 6*time_step, contact.x, velocity);
				}
				else
				{
					effect(SKULL__fireEffect, 6*time_step, contact.x, velocity);
				}
			}
		}
	}
}

void SKULL__hit(ENTITY* ptr)
{
	ptr->ENTITY_HITTHRESHOLD -= time_step;
	if (ptr->HEALTH <= 0)
	{
		ptr->ENTITY_STATE = ENTITY_STATE_DIE;
		ptr->ENTITY_ANIM = 0;
		ent_playsound(ptr,skull_snd_death, 10000);
		set(ptr, PASSABLE);
	}
	else if (ptr->ENTITY_HITTHRESHOLD <= 0)
	{
		ptr->ENTITY_STATE = ENTITY_STATE_WAIT_OR_WALK;
	}
	else
	{
		//freeze animation
		ent_animate(ptr, SKULL_HITANIM, 100, 0);
	}
}

void SKULL__attack(ENTITY* ptr)
{
	if (ptr->ENTITY_ANIM == 0)
	{
		if (num_sounds < 6)
		{
			ent_playsound(ptr,skull_snd_attack, 7000);
		}
	}
	
	ANG_turnToPos(ptr, unit_getTarget(ptr), ptr->SKULL_TURNSPEED, 5);

	ent_animate(ptr, SKULL_WALKANIM, 0, 0);
	ptr->ENTITY_ANIM += time_step/16;
	if(ptr->ENTITY_ANIM > 0.05)
	{
		int i;
		for(i = 0; i < 5; i++)
		{	
			VECTOR velocity;
			vec_set(velocity, &player->x);
			vec_sub(velocity, &ptr->x);
			velocity.z = 5;
			vec_normalize(velocity, 80);
			vec_rotate(velocity, vector(random(10)-5, random(10)-5, 0));
			effect(SKULL__shootEffect, 1, &ptr->x, velocity);
		}
		ptr->SKULL_ANIMSTATEATK += 1;
		ptr->ENTITY_ANIM -= 0.05;

		//do single attack
		if (ptr->SKULL_ANIMSTATEATK > 3)
		{
			if (ptr->SKULL_DIDATTACK == 0)
			{
				ENTITY* victim = unit_getVictim(ptr);
				if (SCAN_IsEntityNear(ptr, victim, ptr->SKULL_ATTACKRANGE))
				{
					//DEBUG
					if (victim == ptr) error ("attacking myself :O");
					unit_setDamage(victim, ptr);
				}
			}
			ptr->SKULL_DIDATTACK = 1;
		}
		else
		{
			ptr->SKULL_DIDATTACK = 0;
		}

		//finish attack
		if(ptr->SKULL_ANIMSTATEATK > 5)
		{
			ptr->SKULL_ANIMSTATEATK = 0;
			ptr->ENTITY_STATE = ENTITY_STATE_WAIT_OR_WALK;
		}
	}	

}

void SKULL__die(ENTITY* ptr)
{
	ptr->ENTITY_ANIM = minv(ptr->ENTITY_ANIM + 4*time_step, 100);
	var animState = (100 - ptr->ENTITY_ANIM ) / 100;	
	vec_set(&ptr->scale_x, vector(animState, animState, animState));
	VECTOR* pos = vector(ptr->x+random(10)-5, ptr->y+random(10)-5, ptr->z+random(10)-5);
	VECTOR* vel = vector(-5-random(10), -2-random(4), 2+random(4));
	effect(PARTICLE_smoke, 6*time_step, pos, vel);

	/* transitions */
	if(animState <= 0)
	{
		reset(ptr, SHADOW);
		ptr->ENTITY_ANIM = 0;
		ptr->ENTITY_STATE = ENTITY_STATE_DEAD;
	}
}

void SKULL__fireParticle(PARTICLE *p)
{
	p.size -= time_step;
	p.alpha -= p.skill_a*time_step*2;
	if(p.alpha <= 0) p.lifespan = 0;
}

void SKULL__fireEffect(PARTICLE *p)
{
	set(p, MOVE | BRIGHT | TRANSLUCENT);
	p.red = 255;
	p.green = 0;
	p.blue = 0;
	p.alpha = 100;
	p.lifespan = 50;
	p.size = 50;
	p.vel_z = 20 + random(30);
	p.gravity = -20.0;
	p.skill_a = 20.0; // fade factor
	p.event = SKULL__fireParticle;
}

void SKULL__smokeEffect(PARTICLE *p)
{
	set(p, MOVE | TRANSLUCENT);
	p.red = 70;
	p.green = 70;
	p.blue = 70;
	p.alpha = 100;
	p.lifespan = 50;
	p.size = 50;
	p.vel_z = 20 + random(30);
	p.gravity = -20.0;
	p.skill_a = 20.0; // fade factor
	p.event = SKULL__fireParticle;
}

void SKULL__shootParticle(PARTICLE *p)
{
	p.size += 50*time_step;
	p.alpha -= p.skill_a*time_step;
	if(p.alpha <= 0) p.lifespan = 0;
}

void SKULL__shootEffect(PARTICLE *p)
{
	set(p, MOVE | BRIGHT | TRANSLUCENT);
	p.red = 255;
	p.green = 0;
	p.blue = 0;
	p.alpha = 100;
	p.lifespan = 100;
	p.size = 100;
	p.skill_a = 20.0; // fade factor
	p.event = SKULL__fireParticle;
}

void smoke_fade_p(PARTICLE* p)
{
	p.alpha -= p.skill_a*time_step;
	if (p.alpha <= 0) p.lifespan = 0;
	
	p.size = minv(p.skill_b, p.size+time_step*3);
}

BMAP* bmp_smoke = "rauch2.tga";

void PARTICLE_smoke(PARTICLE* p)
{
	p.skill_a = 3;
	p.bmap = bmp_smoke;
	p.lifespan = 1200+random(600);
	p.skill_b = 100 + random(50);
	p.size = 20;
	p.alpha = 80;
	vec_scale(p.vel_x, 0.4);
	set(p, TRANSLUCENT | MOVE);
	p.event = smoke_fade_p;
}
