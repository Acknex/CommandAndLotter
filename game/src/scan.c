#include "scan.h"

#define SCAN_CAM_ACTIVEDIST 1000

var SCAN_IsCameraNear(ENTITY* ent)
{
	if (ent == NULL)
		return 0;
		
	if (vec_dist(&camera->x, ent->x) < SCAN_CAM_ACTIVEDIST)
		return 1;
	else
		return 0;
}

var SCAN_IsTargetNear(ENTITY* ent, VECTOR* pos, var distance)
{
	if (ent == NULL)
		return 0;
		
	if (vec_dist(&pos->x, ent->x) < distance)
		return 1;
	else
		return 0;
}

/*var SCAN_IsPlayerInSight(ENTITY* ent, var distance, var fov)
{
    if(SCAN_invisibility)
        return 0;
	if (player == NULL)
		return 0;
		
	if (fov == 0)
		fov = 360;	
		
	me = ent;
	you = NULL;
	var mode = IGNORE_ME | IGNORE_PASSABLE | IGNORE_PASSENTS;
	var vdist = c_trace(&ent->x, &player->x, mode);
	
	if ((vdist >= 0) && (vdist <= distance) && (you != NULL))
	{
		if (you == player)	
		{
			VECTOR vecDir;
			vec_set(&vecDir, &player->x);
			vec_sub(&vecDir, &ent->x);
			
			ANGLE vecAng;
			vec_to_angle(&vecAng, &vecDir);
			ANGLE diffAng;
			vec_diff(&diffAng, &vecAng, &ent->pan);
			
			if (absv(ang(diffAng.pan)) <= fov)
				return 1;
		}
	}
	
	return 0;
}

var SCAN_IsPlayerBehind(ENTITY* ent, var distance)
{
    if(SCAN_invisibility)
        return 0;
	if (player == NULL)
		return 0;
		
		
			VECTOR vecDir;
			vec_set(&vecDir, &player->x);
			vec_sub(&vecDir, &ent->x);
			
			ANGLE vecAng;
			vec_to_angle(&vecAng, &vecDir);
			ANGLE diffAng;
			vec_diff(&diffAng, &vecAng, &ent->pan);
			if ((absv(ang(diffAng.pan)) >= 90) && (vec_dist(&player->x, ent->x) > distance))
				return 1;
			else	
				return 0;
}
*/