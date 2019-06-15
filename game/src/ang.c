#include "ang.h"

var ANG_turnToPos(ENTITY* ptr, VECTOR* pos, var speed, var hysteresis)
{
	ANGLE vecAngle;
	VECTOR vecTemp;
	vec_set(&vecTemp, &pos->x);
	vec_sub(&vecTemp, &ptr->x);
	vec_to_angle(&vecAngle, &vecTemp);
	var x =  ANG_turnToAngle(ptr, vecAngle.pan, speed, hysteresis);
	return x;
}

var ANG_turnToAngle(ENTITY* ptr, var angle, var speed, var hysteresis)
{
	ANGLE* sourceAngle = vector(ang(ptr->pan),0,0);
	ANGLE* targetAngle = vector(ang(angle),0,0);
	ANGLE diffAngle;
	ang_diff(&diffAngle, targetAngle, sourceAngle);
	if (absv(diffAngle.pan) > hysteresis)
	{
		ANGLE* angle;
		DEBUG_VAR(diffAngle.pan, 40);
		if (diffAngle.pan > 0)
		{
			ptr->pan += minv(diffAngle.pan, speed * time_step);
			
		}
		else
		{
			ptr->pan += maxv(diffAngle.pan, -speed * time_step);
		}
		ptr->pan = ang(ptr->pan);
		return 0;
	}
	return 1;
}

/*
var ANG_turnToAngle(ENTITY* ptr, var angle, var speed, var hysteresis)
{
	var diff = ang(ptr->pan-angle);
	if (absv(diff) > hysteresis)
	{
		ANGLE* angle;
		DEBUG_VAR(diff, 40);
		if (diff > 0)
		{
			//ptr->pan += minv(diff, speed * time_step);
			
		}
		else
		{
			//ptr->pan += maxv(diff, -speed * time_step);
		}
		ptr->pan = ang(ptr->pan);
		return 0;
	}
	return 1;
}
*/