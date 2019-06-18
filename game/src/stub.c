#include "unit.h"
#include "spawner.h"
#include "z.h"

ENTITY* fancysputnik;
ENTITY* evilsputnik;
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

void attack()
{
	unit_setTarget(fancysputnik, evilsputnik->x);
	unit_setVictim(fancysputnik, evilsputnik);
}

void stub_init()
{
	on_p = producesputnik;
	on_k = getz;
	on_f = attack;
	fancysputnik = unit_spawn(0, vector(0,0,500), PLAYER_ID_PLAYER);
	evilsputnik = unit_spawn(0, vector(-1500,0,500), PLAYER_ID_AI);
	unit_spawn(0, vector(-1800,0,500), PLAYER_ID_AI);
	unit_spawn(0, vector(-2100,0,500), PLAYER_ID_AI);
	fancytower = spawner_spawn(0, vector(100,1000,500), 0, PLAYER_ID_PLAYER);
	fancyz = z_spawn(vector(3000, 0, 500));
}

void stub_update()
{
}