#include "unit.h"
#include "spawner.h"
#include "z.h"

VECTOR mousevec;
int mouseupdate = 0;
ENTITY* fancysputnik;
ENTITY* fancytower;
ENTITY* fancytarget;
ENTITY* fancyz;
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

void producesputnik()
{
	spawner_produce(fancytower);
}

void getz()
{
	unit_setTarget(fancysputnik, fancyz->x);
	unit_setVictim(fancysputnik, fancyz);
}

void stub_init()
{
	//on_mouse_left = setmousepos;
	on_p = producesputnik;
	on_k = getz;
	fancysputnik = unit_spawn(0, vector(0,0,500), UNIT_PLAYER);
	fancytarget = ent_create(SPHERE_MDL, vector(100,100,500), NULL);
	fancytower = spawner_spawn(0, vector(100,1000,500), SPAWNER_PLAYER);
	fancyz = z_spawn(vector(3000, 0, 500));
	vec_scale (&fancytarget->scale_x, 5);	
}

void stub_update()
{
	return;
	if (mouseupdate == 1)
	{
		mouseupdate = 0;
		stubtoggle = 1 - stubtoggle;
		unit_setTarget(fancysputnik, mousevec);
		if (stubtoggle > 0)
			unit_setVictim(fancysputnik, fancytarget);
		else
			unit_setVictim(fancysputnik, NULL);
	}
	
	if (stubtoggle > 0)
		draw_text("Targeting enemy unit" ,0, 0, vector(255,255,255));
}