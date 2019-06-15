#include "unit.h"

VECTOR mousevec;
int mouseupdate = 0;
ENTITY* fancysputnik;
ENTITY* fancytarget;

void setmousepos()
{
	/*mousevec.x = mouse_pos.x;
	mousevec.y = mouse_pos.y;
	mousevec.z = 3200;
	vec_for_screen(mousevec, camera);*/
	
	vec_set(&mousevec, vector(500+random(500),0,150));
	ANGLE* angle = vector(random(360),0,0);
	vec_rotate(&mousevec, angle);
	vec_set(fancytarget->x, &mousevec);
	mouseupdate = 1;
}

void stub_init()
{
	mouse_mode = 4;
	on_mouse_left = setmousepos;
	fancysputnik = unit_spawn(0, vector(0,0,500));
	fancytarget = ent_create(SPHERE_MDL, vector(100,100,500), NULL);
	vec_scale (&fancytarget->scale_x, 5);	
}

void stub_update()
{
	if (mouseupdate == 1)
	{
		mouseupdate = 0;
		unit_setTarget(fancysputnik, mousevec);
	}
}