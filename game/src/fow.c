#include "fow.h"
#include "jps.h"



BMAP* FogBmap = "fow_fog2.png";

void FogEvent(PARTICLE *p)
{
	p.lifespan = 2;
	p.x = p.skill_a + cosv(p->skill_c*total_ticks)*100+(p.skill_a%100);
	p.y = p.skill_b + cosv(p->skill_d*total_ticks)*100+(p.skill_b%100);

	
	MAP *map = mapGetCurrent();
	TILE *tile = mapGetTileFromVector(map, vector(p->skill_a, p->skill_b, 0));
	
	if(tile->visibility == FOW_SCOUTED) 
	{
		p->alpha -= 10*time_step;
		if(p->alpha <= 0)
		p.lifespan = 0;
	}
	
}

void Fog(PARTICLE *p)
{
	p.bmap = FogBmap;
	p.alpha = 90;
	p.gravity = 0;
	p.size = 600;
	
	var cRand = random(0.4)+0.4;
	p.red = cRand * 255;
	p.green = cRand * 255;
	p.blue = cRand * 255;
	
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
			pos.z = 550;
			
			
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
		if(otherTile)
		{
			VECTOR otherPos;
			mapGetVectorFromTile(map, &otherPos, otherTile);
			
			if(vec_dist(pos, &otherPos) < range)
			otherTile->visibility = FOW_SCOUTED;
		}
	}
}


//var fow_updatedelay = 0;
int fow_calcoffset = 0;
int fow_calcoffsetMAX = 16;
void fow_update()
{
	#ifdef USE_FOW
		MAP *map = mapGetCurrent();
		
		int mapSize = map->size[0]*map->size[1];
		int i;
		for(i = fow_calcoffset; i< mapSize; i = i+fow_calcoffsetMAX)
		{
			TILE *tile = &((map->tiles)[i]);
			if(tile->visibility == FOW_HIDDEN)
			{
				if(mapIsAnyFriendlyUnitNearby(map, tile, FOW_SIGHT_RANGE, PLAYER_ID_PLAYER)) 
				{
					tile->visibility = FOW_SCOUTED;	
				}
			}
		}
		fow_calcoffset++;
		fow_calcoffset = fow_calcoffset%fow_calcoffsetMAX;
		
		/*
		fow_updatedelay += time_step;
		if(fow_updatedelay > 1) 
		{
			fow_updatedelay = 0;
			
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
		}
		*/
		
	#endif
}
