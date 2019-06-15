#include "fow.h"
#include "jps.h"



BMAP* FogBmap = "FogOfWar.tga";


void FogEvent(PARTICLE *p)
{
    p.lifespan = 2;
}

void Fog(PARTICLE *p)
{
    p.bmap = FogBmap;
    p.alpha = 100;
    p.gravity = 0;
    p.size = 100;

    p.flags |= (MOVE | TRANSLUCENT);
    p.event = FogEvent;
}


void FogOfWarTest()
{
    var x, y;
    for(x = -1000; x < 1000; x+=50){
        for(y = -1000; y < 10000; y+=50){
            effect(Fog, 1, vector(x,y,500), nullvector);
        }
    }

}

void fow_open()
{
    FogOfWarTest();
}


void fow_update()
{
	
}
