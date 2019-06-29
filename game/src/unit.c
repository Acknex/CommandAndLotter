#include "sputnik.h"
#include "jps.h"
#include "UnitMangement.h"

#define UNIT_TABLESIZE 36 // UNIT_CLASSES*UNIT_CLASSES Lite-C is stupid. Really.
var unit__dmgtable[UNIT_TABLESIZE] = {
   /*S L E C K Z*/
/*S*/7,9,2,2,5,1,
/*L*/2,5,7,7,5,1,
/*E*/9,5,3,3,5,1,
/*C*/2,5,7,7,5,1,
/*K*/0,0,0,0,0,2,
/*Z*/0,0,0,0,0,0
};
/*
(CBABE)Maschinengewehrinfanterie: ++Infanterie  00Fahrzeug  --Panzer
(EYE)Raketeninfanterie:           --Infanterie  00Fahrzeug  ++Panzer
(LERCHE)Fahrzeug:                 ++Infanterie  00Fahrzeug  --Panzer
(SPUTNIK)Panzer:                  --Infanterie  ++Fahrzeug  ++Panzer
(SKULL): kein Angriff
(Z): kein Angriff, 1 HP
*/

var unit__health[UNIT_CLASSES] = {23,23,23,23,10,1};

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
        if(owner== PLAYER_ID_PLAYER)
            ent = ent_create("sputnik.mdl", pos, Sputnik);
        else
            ent = ent_create("evilSputnik.mdl", pos, Sputnik);
        break;

    case UNIT_LERCHE:
        if(owner== PLAYER_ID_PLAYER)
            ent = ent_create("cEselslerche.mdl", pos, Sputnik);
        else
            ent = ent_create("cEselslerche.mdl", pos, Sputnik);
        break;

    case UNIT_EYE:

        if(owner== PLAYER_ID_PLAYER)
            ent = ent_create("eye.mdl", pos, Sputnik);
        else
            ent = ent_create("eye.mdl", pos, Sputnik);
        break;

    case UNIT_BABE:
        if(owner== PLAYER_ID_PLAYER)
            ent = ent_create("cbabe.mdl", pos, Sputnik);
        else
            ent = ent_create("cbabe.mdl", pos, Sputnik);
        break;

    case UNIT_SKULL:
        if(owner== PLAYER_ID_PLAYER)
            ent = ent_create("whiskas_G.mdl", pos, Skull);
        else
            ent = ent_create("whiskas.mdl", pos, Skull);
        break;

    default:
        break;
    }

	if (ent != NULL)
	{
		ent->OWNER = owner;
		unit_initHealth(ent);
		if (owner == PLAYER_ID_AI)
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

var unit_initHealth(ENTITY* ent)
{
	if (ent != NULL)
	{
		ent->HEALTH = unit__health[clamp(ent->ENTITY_UNITTYPE,0,UNIT_CLASSES-1)];
		ent->MAXHEALTH = ent->HEALTH;
		return ent->HEALTH;
	}
	return 0;
}

var unit_checkHit(ENTITY* ent)
{
	var ret = 0;
	if (ent != NULL)
	{
		if (ent->DAMAGE_HIT > 0)
		{
			//don't count hits while unit is already being hit
			if (ent->ENTITY_STATE != ENTITY_STATE_HIT)
			{
				ent->HEALTH = maxv(0, ent->HEALTH - ent->DAMAGE_HIT);
				//deactivate directly in order to avoid single frame gap
				if (ent->HEALTH <= 0)
					unit_deactivate(ent);
				ret = 1;
			}
			ent->DAMAGE_HIT = 0;
		}
	}
	return ret;
}
var unit_setDamage(ENTITY* victim, ENTITY* attacker)
{
	if (victim != NULL && attacker != NULL)
	{
		var index = UNIT_CLASSES * clamp(attacker->ENTITY_UNITTYPE,0,UNIT_CLASSES-1) + clamp(victim->ENTITY_UNITTYPE,0,UNIT_CLASSES-1);
		victim->DAMAGE_HIT = unit__dmgtable[index];
		victim->DAMAGE_ENT = handle(attacker);

		return unit__dmgtable[index];
	}
	return 0;
}

int unit_getType(ENTITY* ent)
{
	if (ent != NULL)
	{
		return ent->ENTITY_UNITTYPE;
	}
	return -1;
}

ENTITY* unit_findNextVictim(ENTITY* ptr, var unittype)
{
	ptr->ENTITY_VICTIMTYPE = unittype;
	return unit_findNextVictim(ptr);
}

ENTITY* unit_findNextVictim(ENTITY* ptr)
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
		//error("searching");
			if (mapGetNearbyUnitsOfTypeForPos(ptr->x, ptr->ENTITY_VICTIMTYPE, ptr->OWNER, 2000, 1) > 0)
			{
				ent = jpsGetEntityFromUnitArray(0);
				//set new target and victim
				//error(str_for_num(NULL,ent->HEALTH));
				unit_setTarget(ptr, &ent->x);
				unit_setVictim(ptr, ent);
				return ent;
			}
		}
	}
	return NULL;
}

void unit_deactivate(ENTITY* ptr)
{
	UnitMangement_unselect(ptr);
	jpsUnitDestroy(jpsUnitGetFromEntity(ptr));
	ptr->group = 0;
}
