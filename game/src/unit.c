#include "sputnik.h"

var unit_setTarget(ENTITY* ent, VECTOR* pos)
{
	if (ent != NULL)
	{
		if (ent->group == GROUP_PLAYER_UNIT || ent->group == GROUP_ENEMY_UNIT)
		{
			vec_set(ent->UNIT_TARGET, pos);
			return 1;
		}
	}
	return 0;
	
}

VECTOR* unit_getTarget(ENTITY* ent)
{
	return &ent->UNIT_TARGET;
}

var unit_setVictim(ENTITY* ent, ENTITY* victim)
{
	if (ent != NULL)
	{
		if (ent->group == GROUP_PLAYER_UNIT || ent->group == GROUP_ENEMY_UNIT)
		{
			if (victim != NULL)
			{
				//if (victim->group == GROUP_UNIT)
				{
					ent->ENTITY_VICTIM = victim;
					return 1;
				}
			}
			ent->ENTITY_VICTIM = NULL;
		}
	}
	return 0;
}

ENTITY* unit_getVictim(ENTITY* ent)
{
	if (ent != NULL)
	{
		ENTITY* victim = ent->ENTITY_VICTIM;
		return victim;
	}
	return NULL;
}

ENTITY* unit_spawn(int unittype, VECTOR* pos, var owner)
{
	ENTITY* ent = NULL;
	switch (unittype)
	{
		case 0:
			ent = ent_create("sputnik.mdl", vector(0,0,500), Sputnik);
			break;
		
		/*case 1:
			break;
		
		case 2:
			break;*/
		
		default:
			break;
	}
	
	if (ent != NULL)
	{
		unit_setTarget(ent, ent->x);
		unit_setVictim(ent,NULL);
		
		if (owner == UNIT_ENEMY)
			ent->group = GROUP_ENEMY_UNIT;
		else
			ent->group = GROUP_PLAYER_UNIT;		
	}
	
	return ent;
}
