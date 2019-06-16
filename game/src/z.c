#include "global.h"
#include "framework.h"
#include "z.h"
#include "map_loader.h"
#include "materials.h"

#define Z_VALUE 10
#define Z_PER_DAMAGE 0.2
#define Z_VALUE_LEFT skill[20]
#define Z_SIZE skill11

SOUND* z_collect_snd = "z_collect.wav";
var z_amount;

var z_get()
{
	return z_amount;
}


bool z_isSufficient(var amount)
{
	return (amount <= z_amount);
}

bool z_pay(var amount)
{
	if(amount > z_amount)
		return false;
	z_amount -= amount;
	return true;
}

ENTITY* z_spawn(VECTOR* pos)
{
	ENTITY* ent;
    ent = ent_create("Crystalmeth.mdl", pos, Z);
	ent->group = GROUP_ENEMY_UNIT; //should be neutral group
	return ent;
}

void Z()
{
	framework_setup(my, SUBSYSTEM_Z);
    set(my, SHADOW);
    my->Z_SIZE = 2.0 + random(2);
    my->pan = random(360);
    vec_set(my->scale_x, vector(my->Z_SIZE , my->Z_SIZE , my->Z_SIZE ));
	c_setminmax(me);
	my->ENTITY_STATE = ENTITY_STATE_WAIT_OR_WALK;
    my->ENTITY_UNITTYPE = UNIT_Z;
    my->material = matCrystals;
    my->Z_VALUE_LEFT = Z_VALUE;
}

void Z_Init()
{
	z_amount = 1000;
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

        ptr->z = maploader_get_height(ptr->x) - ptr->min_z;

	}
}

void Z__hit(ENTITY* ptr)
{
	z_amount += minv(Z_VALUE, ptr->DAMAGE_HIT*Z_PER_DAMAGE);

	ptr->Z_VALUE_LEFT -= ptr->DAMAGE_HIT*Z_PER_DAMAGE;
    ptr->DAMAGE_HIT = 0;

    if(ptr->Z_VALUE_LEFT <= 0)
    {
		ptr->ENTITY_STATE = ENTITY_STATE_DIE;
		ptr->ENTITY_ANIM = 100;

        if (num_sounds < 6)
        {
            var hndl = snd_play(z_collect_snd, 50, 0);
            snd_tune(hndl,0,random(10)+95,0);
        }
	}
	else
		ptr->ENTITY_STATE = ENTITY_STATE_WAIT_OR_WALK;
}

void Z__wait(ENTITY* ptr)
{
    ptr->red = ptr->green = ptr->blue = 100 + (sin(ptr->x + ptr->y + total_ticks * 0.1) * 0.5 + 0.5) * 100.0;
}


void Z__die(ENTITY* ptr)
{
	ptr->ENTITY_ANIM -= 15 * time_step;
	ptr->scale_x = ptr->Z_SIZE * maxv(ptr->ENTITY_ANIM, 0) * 0.01;
	ptr->scale_y = ptr->scale_x;

	/* transitions */
	if(ptr->ENTITY_ANIM <= 0)
	{
		ptr->ENTITY_STATE = ENTITY_STATE_DEAD;
		set(ptr, PASSABLE);
		framework_remove(ptr);
	}
}
