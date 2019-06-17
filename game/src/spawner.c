#include "global.h"
#include "framework.h"
#include "spawner.h"
#include "map_loader.h"
#include "unit.h"
#include "fow.h"
#include "unitmangement.h"

#define SPAWNER_QUEUE skill21
#define SPAWNER_PROGRESS skill22
#define SPAWNER_BUILDTIMER skill23
#define SPAWNER_DIETIMER skill24
#define SPAWNER_HITTHRESHOLD skill27
#define SPAWNER_FIREPARTICLES skill28
#define SPAWNER_DEBRISPARTICLES skill29
#define SPAWNER_SMOKEPARTICLES skill30
#define SPAWNER_WIREFRAME skill31
#define SPAWNER_SPAWNANGLE skill32


#define SPAWNER_BASEX skill70			// Position nach dem erstellen
#define SPAWNER_BASEY skill71
#define SPAWNER_BASEZ skill72


#define SPAWNER_ACTIVEANIM "stand"
#define SPAWNER_PRODUCEANIM "produce"
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
#define SPAWNER_LOS 2000

#define SPAWNER_LOS 2000

SOUND* spawner_spawn_snd = "factory.wav";
SOUND* spawner_destroy_snd = "factory_destroyed.wav";

MATERIAL * building_wireframe_material =
{
  flags = AUTORELOAD | TRANSLUCENT;
  effect = "building_wireframe.fx";
}

MATERIAL * building_material =
{
  flags = AUTORELOAD | PASS_SOLID;
  effect = "building.fx";
}

ENTITY* spawner_spawn(int unittype, VECTOR* pos, var angle, var owner)
{
	ENTITY* ent;
   ENTITY *wireframe;
	switch (unittype)
	{
        case UNIT_SPUTNIK:
            ent = ent_create("sputnik_trash_press.mdl", pos, Spawner);
            wireframe = ent_create("sputnik_trash_press_wf.mdl", pos, NULL);
            break;
        case UNIT_LERCHE:
            ent = ent_create("lark_farm.mdl", pos, Spawner);
            wireframe = ent_create("lark_farm_wireframe.mdl", pos, NULL);
            break;
        case UNIT_EYE:
            ent = ent_create("eye_tree_you.mdl", pos, Spawner);
            wireframe = ent_create("eye_tree_you_wireframe.mdl", pos, NULL);
            break;
        case UNIT_BABE:
            ent = ent_create("bank_of_zorro.mdl", pos, Spawner);
            wireframe = ent_create("bank_of_zorro_wireframe.mdl", pos, NULL);
            break;
	}

	if (ent != NULL)
	{
		ent->pan = angle;
		wireframe->pan = angle;
		mapSetTileValueAtPos3D(mapGetCurrent(), pos, 1); // 1 == solid, non-traversable
		mapJPSUpdate(mapGetCurrent());

		ent->ENTITY_UNITTYPE = unittype;
		if (owner == SPAWNER_ENEMY)
		{
			ent->group = GROUP_ENEMY_SPAWNER;
		}
		else
		{
			ent->group = GROUP_PLAYER_SPAWNER;
			snd_play(spawner_spawn_snd, 100, 0);
			fov_uncover(pos, SPAWNER_LOS);
		}

		ent->material = building_material;

		if(wireframe != NULL)
		{
			wireframe->material = building_wireframe_material;
			reset(wireframe, SHADOW);
			ent->SPAWNER_WIREFRAME = wireframe;
		}
	}

	return ent;
}

//TODO: zloty interface currently is for player only, needs to support enemy as well
var spawner_produce(ENTITY* ent)
{
	if (ent != NULL)
	{
		if (ent->group == GROUP_ENEMY_SPAWNER || ent->group == GROUP_PLAYER_SPAWNER)
		{
			if(z_isSufficient(spawner_unit_cost[ent->ENTITY_UNITTYPE]))
			{
				z_pay(spawner_unit_cost[ent->ENTITY_UNITTYPE]);
				ent->SPAWNER_QUEUE++;
			}
			return ent->SPAWNER_QUEUE;
		}
	}
	return 0;
}

var spawner_getQueue(ENTITY* ent)
{
	if (ent != NULL)
	{
		if (ent->group == GROUP_ENEMY_SPAWNER || ent->group == GROUP_PLAYER_SPAWNER)
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
		if (ent->group == GROUP_ENEMY_SPAWNER || ent->group == GROUP_PLAYER_SPAWNER)
		{
			if (ent->ENTITY_STATE == SPAWNER_STATE_PRODUCE)
				return 1 - (ent->SPAWNER_BUILDTIMER / SPAWNER_BUILDTIME);
		}
	}
	return 0;
}

var spawner_getHealth(ENTITY* ent)
{
	return ent->HEALTH / ent->MAXHEALTH;
}

void Spawner()
{
   framework_setup(my, SUBSYSTEM_SPAWNER);
	my->HEALTH = 50;
	my->MAXHEALTH = my->HEALTH;
	set(my, SHADOW);
    reset(my, TRANSLUCENT);
	c_setminmax(me);
	my->ENTITY_STATE = SPAWNER_STATE_CONSTRUCT;

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
	ptr->ENTITY_HITTHRESHOLD = 3;

	ptr->HEALTH = maxv(0, ptr->HEALTH - ptr->DAMAGE_HIT);
	ptr->DAMAGE_HIT = 0;

	if (ptr->HEALTH <= 0)
	{
		UnitMangement_unselect(ptr);
		ptr->ENTITY_STATE = SPAWNER_STATE_DIE;
		ptr->ENTITY_ANIM = 0;
		snd_play(spawner_destroy_snd, 100, 0);
	}
}

void SPAWNER__hitcheck(ENTITY* ptr)
{
	if (ptr->ENTITY_HITTHRESHOLD > 0)
	{
		ptr->ENTITY_HITTHRESHOLD -= time_step;

		if (ptr->ENTITY_HITTHRESHOLD <= 0)
		{
			ptr->ENTITY_HITTHRESHOLD = 0;
		}
	}
}


void SPAWNER__construct(ENTITY* ptr)
{
	ptr->SPAWNER_PROGRESS += 5 * time_step;

	var percentage = ptr->SPAWNER_PROGRESS * (ptr.max_z + 200) / 100;
	ptr->skill41 = floatv(percentage);
	ptr->skill42 = floatv(ptr->max_x * 0.5);
	ptr->skill43 = floatv(clamp((80.0 - ptr->SPAWNER_PROGRESS)/20.0, 0.0, 1.0));

	ptr->skill45 = floatv(ptr->x);
	ptr->skill46 = floatv(ptr->z);
	ptr->skill47 = floatv(ptr->y);

	ENTITY *wireframe = ptr->SPAWNER_WIREFRAME;
	if(wireframe != NULL)
	{
		wireframe->skill41 = floatv(percentage);

		wireframe->skill45 = floatv(wireframe->x);
		wireframe->skill46 = floatv(wireframe->z);
		wireframe->skill47 = floatv(wireframe->y);
	}

	if (ptr->SPAWNER_PROGRESS >= 100)
	{
		ptr->SPAWNER_PROGRESS = 100;
		ptr->ENTITY_STATE = SPAWNER_STATE_ACTIVE;
		ptr->SPAWNER_WIREFRAME = NULL;
		ptr_remove(wireframe);
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
	ent_animate(ptr, SPAWNER_PRODUCEANIM, clamp(progress, 0, 100), 0);

	if (ptr->SPAWNER_BUILDTIMER <= 0)
	{
		ptr->SPAWNER_BUILDTIMER = SPAWNER_BUILDTIME;
		ptr->SPAWNER_QUEUE--;

		//meh.
		var owner;
		if (ptr->group == GROUP_ENEMY_SPAWNER)
			owner = UNIT_ENEMY;
		else
			owner = UNIT_PLAYER;
		ptr->SPAWNER_SPAWNANGLE += 137.5;
		ptr->SPAWNER_SPAWNANGLE %= 360;
		VECTOR* targetPos = vector(400,0,0);
		VECTOR* angle = vector(ang(ptr->SPAWNER_SPAWNANGLE), 0, 0);
		vec_rotate(targetPos, angle);
		vec_add(targetPos, ptr->x);
		unit_spawn(ptr->ENTITY_UNITTYPE, ptr->x, targetPos, owner);
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
	p->skill_a = random(20) + 10;
	p->bmap = bmp_smoke;
	p->lifespan = 9000;
	p->skill_b = 1000 + random(50);
	p->size = 200;
	p->alpha = 80;
	//vec_scale(p->vel_x, 0.4);
	p->vel_x = 20-random(40);
	p->vel_y = 20-random(40);
	p->vel_z = 20 - random(30);
	set(p, TRANSLUCENT | MOVE);
	p.event = SPAWNER_smoke_fade_p;
}

void SPAWNER_debris(PARTICLE* p)
{
	p->lifespan = 50;
	p->size = 100 + random(200);
	p->alpha = 80;
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
	p->skill_a-=time_step * 3;
	p->alpha = maxv(0, minv(100, p->skill_a));
	p->size -= time_step * p->skill_b;
	p->green = minv(255, p->green + time_step * p->skill_b * 2);
}

BMAP* bmp_fire = "rauch2.tga";

void SPAWNER_fire(PARTICLE* p)
{
	p->skill_a = random(30) + 60;
	p->lifespan = 90;
	p->skill_b = random(4) + 2;
	p->size = 200;
	p->alpha = 80;
	p->red = 255;
	p->green = 80;
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

	if (deathfactor < 0.1)
	{ // DEBRIS
		VECTOR* pos = vector(ptr->SPAWNER_BASEX+random(100)-50, ptr->SPAWNER_BASEY+random(100)-50, ptr->SPAWNER_BASEZ+random(100)-50);
		effect(SPAWNER_debris, 12*time_step, pos, nullvector);
	}

	if (deathfactor < 0.3)
	{ // FIRE
		VECTOR* pos = vector(ptr->SPAWNER_BASEX+random(100)-50, ptr->SPAWNER_BASEY+random(100)-50, ptr->SPAWNER_BASEZ+random(100)-50);
		effect(SPAWNER_fire, 30*time_step, pos, nullvector);
	}

	if (deathfactor < 0.7)
	{ //SMOKE
		VECTOR* pos = vector(ptr->SPAWNER_BASEX+random(100)-50, ptr->SPAWNER_BASEY+random(100)-50, ptr->SPAWNER_BASEZ+random(100)-50);
		effect(SPAWNER_smoke, 60*time_step, pos, nullvector);
	}


	//VECTOR* pos = vector(ptr->x+random(10)-5, ptr->y+random(10)-5, ptr->z+random(10)-5);
	//VECTOR* vel = vector(-5-random(10), -2-random(4), 2+random(4));
	//effect(PARTICLE_smoke, 6*time_step, pos, vel);

	/* transitions */
	if(ptr->SPAWNER_DIETIMER <= 0)
	{
		mapSetTileValueAtPos3D(mapGetCurrent(), ptr->x, 0); // 1 == solid, non-traversable
		mapJPSUpdate(mapGetCurrent());
		reset(ptr, SHADOW);
		ptr->SPAWNER_DIETIMER  = 0;
		ptr->ENTITY_STATE = SPAWNER_STATE_DEAD;
	}
}
