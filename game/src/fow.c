#include "fow.h"
#include "jps.h"



BMAP* FogBmap = "fow_fog2.png";
VECTOR MousePosFog;

void FogEvent(PARTICLE *p)
{
    p.lifespan = 2;
    p.x = p.skill_a + sinv(p->skill_c*total_ticks)*100+(p.skill_a%100);
    p.y = p.skill_b + sinv(p->skill_d*total_ticks)*100+(p.skill_b%100);

	
	MAP *map = mapGetCurrent();
	TILE *tile = mapGetTileFromVector(map, vector(p->skill_a, p->skill_b, 0));
	
    if(tile->visibility == FOW_SCOUTED){
        p.lifespan = 0;
    }
}

int fov_px, fov_py;

void Fog(PARTICLE *p)
{
    p.bmap = FogBmap;
    p.alpha = 90;
    p.gravity = 0;
    p.size = 600;

    p.flags |= (MOVE | TRANSLUCENT);
    p.event = FogEvent;
    
    p.skill_a = p.x;
    p.skill_b = p.y;
    
    p->skill_c = random(0.5)+0.5;
    p->skill_d = random(0.5)+0.5;
}



void fow_open()
{
#ifdef USE_FOW
	MAP *map = mapGetCurrent();

	int x, y;
	for(x = 0; x<map->size[0]; ++x)
		for(y = 0; y<map->size[1]; ++y)
		{
			TILE *tile = mapTileGet(map, x,y);
			tile->visibility = FOW_HIDDEN;
			
			VECTOR pos;
			mapGetVectorFromTile(map, &pos, tile);
			pos.z = 500;
            effect(Fog, 1, pos, nullvector);
		}
#endif
}

void fow_update()
{
#ifdef USE_FOW
	MAP *map = mapGetCurrent();

	int x, y;
	for(x = 0; x<map->size[0]; ++x)
		for(y = 0; y<map->size[1]; ++y)
		{
			TILE *tile = mapTileGet(map, x,y);
			if(tile->visibility == FOW_HIDDEN){
				if(mapIsAnyFriendlyUnitNearby(map, tile, FOW_SIGHT_RANGE, 0)) {
					tile->visibility = FOW_SCOUTED;	
				}
			}
				
		}


    //PosToMap(MousePosFog,mouse_pos.x,mouse_pos.y);
    //MousePosFog.z = 500;
    
#endif
}
