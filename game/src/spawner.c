#include "global.h"
#include "framework.h"
#include "spawner.h"
#include "enemy_hit.h"
//#include "particle.h"
#include "map_loader.h"

#define SPAWNER_QUEUE skill21
#define SPAWNER_PROGRESS skill22
#define SPAWNER_BUILDTIMER skill23
#define SPAWNER_DIETIMER skill24
#define SPAWNER_HITTHRESHOLD skill27
#define SPAWNER_FIREPARTICLES skill28
#define SPAWNER_DEBRISPARTICLES skill29
#define SPAWNER_SMOKEPARTICLES skill30

#define SPAWNER_BASEX skill40			// Position nach dem erstellen
#define SPAWNER_BASEY skill41
#define SPAWNER_BASEZ skill42


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

#define SPAWNER_MAXFIRE 40
#define SPAWNER_MAXDEBRIS 5
#define SPAWNER_MAXSMOKE 90

ENTITY* spawner_spawn(int spawnertype, VECTOR* pos, var owner)
{
	ENTITY* ent;
	switch (spawnertype)
	{
		case 0:
			ent = ent_create("the_tower.mdl", pos, Spawner);
			break;
	}

	if (ent != NULL)
	{
		if (owner == SPAWNER_ENEMY)
			ent->group = GROUP_ENEMY_SPAWNER;
		else
			ent->group = GROUP_PLAYER_SPAWNER;		
	}
	
	return ent;
}

var spawner_produce(ENTITY* ent)
{
	if (ent != NULL)
	{
		if (ent->group = GROUP_ENEMY_SPAWNER || ent->group == GROUP_PLAYER_SPAWNER)
		{
			ent->SPAWNER_QUEUE++;
			return ent->SPAWNER_QUEUE;
		}
	}
	return 0;
}

var spawner_getQueue(ENTITY* ent)
{
	if (ent != NULL)
	{
		if (ent->group = GROUP_ENEMY_SPAWNER || ent->group == GROUP_PLAYER_SPAWNER)
		{
			return ent->SPAWNER_QUEUE;
		}
	}
	return 0;
}

var spawner_getProgress(ENTITY* ent)
{
	if (ent != NULL)
	{
		if (ent->group = GROUP_ENEMY_SPAWNER || ent->group == GROUP_PLAYER_SPAWNER)
		{
			return 1 - (ent->SPAWNER_BUILDTIMER / SPAWNER_BUILDTIME);
		}
	}
	return 0;
}

void Spawner()
{
   framework_setup(my, SUBSYSTEM_SPAWNER);
	my->HEALTH = 23;//TODO HOOK TO UNIT SYSTEM
	ENEMY_HIT_init(my);
	set(my, SHADOW);
	c_setminmax(me);
	my->ENTITY_STATE = SPAWNER_STATE_CONSTRUCT;
	//my->ENTITY_STATE = SPAWNER_STATE_DIE;

	my->SPAWNER_QUEUE = 0;
	my->SPAWNER_BUILDTIMER = SPAWNER_BUILDTIME;
	my->SPAWNER_DIETIMER = SPAWNER_DIETIME;

	my->z = maploader_get_height(my->x) - my->min_z;			
	my->SPAWNER_BASEX = my->x;
	my->SPAWNER_BASEY = my->y;
	my->SPAWNER_BASEZ = my->z;
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
	var progress = 100 * (1 - (ptr->SPAWNER_BUILDTIMER / SPAWNER_BUILDTIME));
	ent_animate(ptr, SPAWNER_PRODUCEANIM, progress, 0);
	
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

void SPAWNER_smoke_fade_p(PARTICLE* p)
{
	p->skill_a-=time_step;
	p->alpha = maxv(0, minv(100, p->skill_a));
	if (p->alpha <= 0) p->lifespan = 0;

	p->size = minv(p->skill_b, p->size+time_step*3);
}

BMAP* bmp_smoke = "rauch2.tga";

void SPAWNER_smoke(PARTICLE* p)
{
	p.skill_a = random(30) + 10;
	p.bmap = bmp_smoke;
	p.lifespan = 9000;
	p.skill_b = 1000 + random(50);
	p.size = 200;
	p.alpha = 80;
	//vec_scale(p->vel_x, 0.4);
	p->vel_x = 20-random(40);
	p->vel_y = 20-random(40);
	p->vel_z = 20 - random(30);
	set(p, TRANSLUCENT | MOVE);
	p.event = SPAWNER_smoke_fade_p;
}

void SPAWNER_debris(PARTICLE* p)
{
	p.lifespan = 50;
	p.size = 100 + random(200);
	p.alpha = 80;
	p->gravity = 4;
	p->red = 0;
	p->green = 0;
	p->blue = 0;
	p->vel_x = 80-random(160);
	p->vel_y = 80-random(160);
	p->vel_z = 20 + random(30);
	set(p, TRANSLUCENT | MOVE);
	p->event = NULL;
}


void SPAWNER_fire_fade_p(PARTICLE* p)
{
	p->skill_a-=time_step;
	p->alpha = maxv(0, minv(100, p->skill_a));
	if (p->alpha <= 0) p->lifespan = 0;

	p->size = minv(p->skill_b, p->size+time_step*3);
}

BMAP* bmp_fire = "rauch2.tga";

void SPAWNER_fire(PARTICLE* p)
{
	p.skill_a = random(30) + 10;
	p.bmap = bmp_smoke;
	p.lifespan = 9000;
	p.skill_b = 1000 + random(50);
	p.size = 200;
	p.alpha = 80;
	//vec_scale(p->vel_x, 0.4);
	p->vel_x = 20-random(40);
	p->vel_y = 20-random(40);
	p->vel_z = 20 - random(30);
	set(p, TRANSLUCENT | MOVE);
	p.event = SPAWNER_fire_fade_p;
}


void SPAWNER__die(ENTITY* ptr)
{
	ptr->SPAWNER_DIETIMER -= time_step;

	var deathfactor = 1.0 - (ptr->SPAWNER_DIETIMER / SPAWNER_DIETIME);

	ptr->z = ptr->SPAWNER_BASEZ - deathfactor * 700.0;
	ptr->roll = deathfactor * 15.0;

	if (deathfactor < 0.2)
	{ // DEBRIS
		VECTOR* pos = vector(ptr->SPAWNER_BASEX+random(100)-50, ptr->SPAWNER_BASEY+random(100)-50, ptr->SPAWNER_BASEZ+random(100)-50);
		effect(SPAWNER_debris, 12*time_step, pos, nullvector);
	}

	if (deathfactor < 0.4)
	{ // FIRE
		VECTOR* pos = vector(ptr->SPAWNER_BASEX+random(100)-50, ptr->SPAWNER_BASEY+random(100)-50, ptr->SPAWNER_BASEZ+random(100)-50);

	}

	//SMOKE
	VECTOR* pos = vector(ptr->SPAWNER_BASEX+random(100)-50, ptr->SPAWNER_BASEY+random(100)-50, ptr->SPAWNER_BASEZ+random(100)-50);
	effect(SPAWNER_smoke, 60*time_step, pos, nullvector);


	//VECTOR* pos = vector(ptr->x+random(10)-5, ptr->y+random(10)-5, ptr->z+random(10)-5);
	//VECTOR* vel = vector(-5-random(10), -2-random(4), 2+random(4));
	//effect(PARTICLE_smoke, 6*time_step, pos, vel);

	/* transitions */
	if(ptr->SPAWNER_DIETIMER <= 0)
	{
		reset(ptr, SHADOW);
		ptr->SPAWNER_DIETIMER  = 0;
		ptr->ENTITY_STATE = SPAWNER_STATE_DEAD;
	}
}
