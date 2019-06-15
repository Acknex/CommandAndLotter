#include "fow.h"
#include "jps.h"



BMAP* FogBmap = "FogOfWar.tga";
VECTOR MousePosFog;
var FogMove;

void FogEvent(PARTICLE *p)
{
    p.lifespan = 2;
    p.x = p.skill_a + FogMove*100+(p.skill_a%100);

    if(vec_dist(vector(p.skill_a, p.skill_b, 500),MousePosFog) < 150){
        p.lifespan = 0;
    }
}

void Fog(PARTICLE *p)
{
    p.bmap = FogBmap;
    p.alpha = 90;
    p.gravity = 0;
    p.size = 100;

    p.flags |= (MOVE | TRANSLUCENT);
    p.event = FogEvent;
    p.skill_a = p.x;
    p.skill_b = p.y;
}


void FogOfWarTest()
{
    max_particles = 1000000;
    var x, y;
    for(x = -1000; x < 1000; x+=100){
        for(y = -1000; y < 1000; y+=100){
            effect(Fog, 1, vector(x,y,500), nullvector);
        }
    }

}

bool fow_active = 0;


#define FOW_HIDDEN 0
#define FOW_SCOUTED 1
#define FOW_SIGHT_RANGE 1

void fow_open()
{
    if(key_f){
    	fow_active = 1;
        FogOfWarTest();
        
		MAP *map = mapGetCurrent();
	
		int x, y;
		for(x = 0; x<map->size[0]; ++x)
			for(y = 0; y<map->size[1]; ++y)
			{
				TILE *tile = mapTileGet(map, x,y);
				tile->visibility = FOW_HIDDEN;
			}
    }
}

void fow_update()
{
	if(fow_active)
	{
		MAP *map = mapGetCurrent();
	
		int x, y;
		for(x = 0; x<map->size[0]; ++x)
			for(y = 0; y<map->size[1]; ++y)
			{
				TILE *tile = mapTileGet(map, x,y);
				if(tile->visibility == FOW_HIDDEN)
					if(mapIsAnyFriendlyUnitNearby(map, tile, FOW_SIGHT_RANGE, SPAWNER_PLAYER))
						tile->visibility = FOW_SCOUTED;	
			}
	}


    PosToMap(MousePosFog,mouse_pos.x,mouse_pos.y);
    MousePosFog.z = 500;
    FogMove = sinv(total_ticks);
}
