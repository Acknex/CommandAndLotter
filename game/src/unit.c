#include "sputnik.h"

var unit_setTarget(ENTITY* ent, VECTOR* pos)
{
	VECTOR target2D;
	MAP* map = mapGetCurrent();
	mapGetVector2DFromVector3D(map, target2D, pos);
	
	//cprintf2("\n unit_setTarget(%p): group(%d)", ent, ent->group);
	
	if (ent != NULL)
	{
		if (ent->group == GROUP_PLAYER_UNIT || ent->group == GROUP_ENEMY_UNIT)
		{
			vec_set(ent->UNIT_TARGET, pos);
			unitSetTargetFromVector2D(map, jpsUnitGetFromEntity(ent), target2D);
			unit_setVictim(ent, NULL);
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
					ent->ENTITY_VICTIM = handle(victim);
					ent->ENTITY_VICTIMTYPE = victim->ENTITY_UNITTYPE;
					return 1;
				}
			}
			ent->ENTITY_VICTIMTYPE = UNIT_INVALID;
			ent->ENTITY_VICTIM = NULL;	
		}
	}
	return 0;
}

ENTITY* unit_getVictim(ENTITY* ent)
{
	if (ent != NULL)
	{
		ENTITY* victim = ptr_for_handle(ent->ENTITY_VICTIM);
		if (victim != NULL)
		{
			if (victim->ENTITY_STATE == ENTITY_STATE_DIE || victim->ENTITY_STATE == ENTITY_STATE_DEAD)
				return NULL;
		}
		return victim;
	}
	return NULL;
}

ENTITY* unit_spawn(int unittype, VECTOR* pos, var owner)
{
	return unit_spawn(unittype, pos, pos, owner);
}

ENTITY* unit_spawn(int unittype, VECTOR* pos, VECTOR* targetPos, var owner)
{
	ENTITY* ent = NULL;
	switch (unittype)
	{
		case UNIT_SPUTNIK:
			ent = ent_create("sputnik.mdl", pos, Sputnik);
			break;
		
		case UNIT_LERCHE:
			ent = ent_create("cEselslerche.mdl", pos, Sputnik);
			break;
		
		case UNIT_EYE:
			ent = ent_create("eye.mdl", pos, Sputnik);
			break;
		
		case UNIT_BABE:
			ent = ent_create("cbabe.mdl", pos, Sputnik);
			break;
		
		default:
			break;
	}
	
	if (ent != NULL)
	{
		
		if (owner == UNIT_ENEMY)
		{
			ent->group = GROUP_ENEMY_UNIT;
			ent->SK_ENTITY_JPS_POINTER_TO_UNIT_STRUCT = jpsUnitCreate(PLAYER_ID_AI, unittype, ent);
		}
		else
		{
			ent->group = GROUP_PLAYER_UNIT;		
			ent->SK_ENTITY_JPS_POINTER_TO_UNIT_STRUCT = jpsUnitCreate(PLAYER_ID_PLAYER, unittype, ent);
		}

		ent->ENTITY_UNITTYPE = unittype;
		unit_setTarget(ent, targetPos);
		unit_setVictim(ent,NULL);
	}
	
	return ent;
}

var unit_getHealth(ENTITY* ent)
{
	return ent->HEALTH / ent->MAXHEALTH;
}

void unit_setDamage(ENTITY* ent, var damage)
{
	if (ent != NULL)
	{
		ent->DAMAGE_HIT = damage;
	}
}

int unit_getType(ENTITY* ent)
{
	if (ent != NULL)
	{
		return ent->ENTITY_UNITTYPE;	
	}
	return -1;
}

void unit_findNextVictim(ENTITY* ptr, var unittype)
{
	ptr->ENTITY_VICTIMTYPE = unittype;
	unit_findNextVictim(ptr);	
}

void unit_findNextVictim(ENTITY* ptr)
{
	/* following requirements have to be met for auto-picking of next victim
	   1) no victim active
	   2) unit type of last victim is still valid
	   
	   if unit type of last victim is cleared, no new victim is searched
	 */
	
	if (unit_getVictim(ptr) == NULL && ptr->ENTITY_VICTIMTYPE != UNIT_INVALID)
	{
		cprintf0("\n unit__findNextVictim: TRYFIND...");
		ENTITY* ent;
		if(ptr->ENTITY_VICTIMTYPE == UNIT_Z)
		{
			ent = z_findNear(&ptr->x, 2000);
			if(ent)
			{
				unit_setTarget(ptr, &ent->x);
				unit_setVictim(ptr, ent);
			}
		}
		else
		{
			//meh.
			var owner;
			if (ptr->group == GROUP_ENEMY_UNIT)	
				owner = UNIT_ENEMY;
			else
				owner = UNIT_PLAYER;
		
			if (mapGetNearbyUnitsOfTypeForPos(ptr->x, ptr->ENTITY_VICTIMTYPE, owner, 2000, 1) > 0)
			{
				ent = jpsGetEntityFromUnitArray(0);
				//set new target and victim
				unit_setTarget(ptr, &ent->x);
				unit_setVictim(ptr, ent);
			}
		}
	}
}

