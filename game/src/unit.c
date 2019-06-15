#include "sputnik.h"

var unit_setTarget(ENTITY* ent, VECTOR* pos)
{
	if (ent != NULL)
	{
		if (ent->group == GROUP_UNIT)
		{
			vec_set(ent->UNIT_TARGET, pos);
			return 1;
		}
	}
	return 0;
	
}

ENTITY* unit_spawn(int unittype, VECTOR* pos)
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
	
	return ent;
}
