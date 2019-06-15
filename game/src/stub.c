#include "unit.h"
#include "spawner.h"

VECTOR mousevec;
int mouseupdate = 0;
ENTITY* fancysputnik;
ENTITY* fancytower;
ENTITY* fancytarget;
int stubtoggle = 0;

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
	fancysputnik = unit_spawn(0, vector(0,0,500), UNIT_PLAYER);
	fancytarget = ent_create(SPHERE_MDL, vector(100,100,500), NULL);
	fancytower = spawner_spawn(0, vector(100,1000,500), SPAWNER_PLAYER);
	vec_scale (&fancytarget->scale_x, 5);	
}

void stub_update()
{
	if (mouseupdate == 1)
	{
		mouseupdate = 0;
		stubtoggle = 1 - stubtoggle;
		//unit_setTarget(fancysputnik, mousevec);
		if (stubtoggle > 0)
			unit_setVictim(fancysputnik, fancytarget);
		else
			unit_setVictim(fancysputnik, NULL);
	}
	
	if (stubtoggle > 0)
		draw_text("Targeting enemy unit" ,0, 0, vector(255,255,255));
}