#include "unit.h"
#include "spawner.h"
#include "z.h"

ENTITY* fancysputnik;
ENTITY* fancytower;
ENTITY* fancyz;

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
	fancytower = spawner_spawn(0, vector(100,1000,500), SPAWNER_PLAYER);
	fancyz = z_spawn(vector(3000, 0, 500));
}

void stub_update()
{
}