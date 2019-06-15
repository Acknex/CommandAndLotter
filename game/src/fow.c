#include "fow.h"
#include "jps.h"



BMAP* FogBmap = "fow_fog2.png";

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
			
			VECTOR pos;
			mapGetVectorFromTile(map, &pos, tile);
			pos.z = 500;
			
			
			tile->visibility = FOW_HIDDEN;
            effect(Fog, 1, pos, nullvector);
		}
#endif
}


void fov_uncover(VECTOR *pos, var range)
{
	MAP *map = mapGetCurrent();
	TILE *tile = mapGetTileFromVector(map, pos);
	
	int iRange = floor(range / map->tileSize + 0.5);
	
	int i,j;
	for(i = tile->pos[0]-iRange; i <= tile->pos[0]+iRange; i++)
		for(j = tile->pos[1]-iRange; j <= tile->pos[1]+iRange; j++)
		{
			TILE *otherTile = mapTileGet(map, i, j);
			VECTOR otherPos;
			apGetVectorFromTile(map, &otherPos, otherTile)
			
			if(vec_dist(pos, otherPos) < range)
				otherTile->visibility = FOW_SCOUTED;
		}
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
				if(mapIsAnyFriendlyUnitNearby(map, tile, FOW_SIGHT_RANGE, PLAYER_ID_PLAYER)) 
				{
					tile->visibility = FOW_SCOUTED;	
				}
			}
				
		}
#endif
}
