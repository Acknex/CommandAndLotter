#include "fow.h"
#include "jps.h"


#define FOW_LIGHTNING_RANGE 2.5
#define FOW_LIGHTNING_DURATION 2.0

BMAP* FogBmap = "fow_fog4.png";
int fow_lightningX[] = {-100,-100,-100};
int fow_lightningY[] = {-100,-100,-100};
int fow_numLigntnings = 0;
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
	else
	{
		p.flags &= ~BRIGHT;
		int i;
		for(i=0; i<fow_numLigntnings; ++i)
		{
			VECTOR dist;
			dist.x = tile->pos[0]-fow_lightningX[i];
			dist.y = tile->pos[1]-fow_lightningY[i];
			dist.z = 0;
			if(vec_length(&dist)<FOW_LIGHTNING_RANGE)
				p.flags |= BRIGHT;
		}
		
	}
        
}

void Fog(PARTICLE *p)
{
    p.bmap = FogBmap;
    p.alpha = 90;
    p.gravity = 0;
    p.size = 600;
    
    var cRand = random(0.9)+0.1;
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

bool fow_hasDirectLOS(MAP *map, VECTOR* t1, VECTOR *t2)
{
	VECTOR tp1, tp2;
	
	//mapGetVectorFromTile(map, &tp1, t1);
	//mapGetVectorFromTile(map, &tp2, t2);
	vec_set(&tp1, t1);
	vec_set(&tp2, t2);
	
	tp1.z += 300;
	tp2.z += 300;
	if(maploader_trace(&tp1, &tp2) == NULL)				
		return 1;
	return 0;
}

int fow_isTileInLOS(MAP* map, TILE* sourceTile, int range, int playerNumber)
{
	if(!sourceTile) return NULL;
	VECTOR pos;
	mapGetVectorFromTile(map, &pos, sourceTile);
	int iRange = floor(range / map->tileSize + 0.5);
	int i,j;
	for(i = sourceTile->pos[0]-iRange; i <= sourceTile->pos[0]+iRange; i++)
	{
		for(j = sourceTile->pos[1]-iRange; j <= sourceTile->pos[1]+iRange; j++)
		{
			TILE* tile = mapTileGet(map, i, j);
				
			int currentPlayer;
			if(!tile) 
				continue;
				
			VECTOR otherPos;
			mapGetVectorFromTile(map, &otherPos, tile);
			if(vec_dist(pos, &otherPos) > range)
				continue;
			
			if(!tile->numUnits[playerNumber]) 
				continue;
			if(fow_hasDirectLOS(map, &pos, &otherPos))			
				return 1;
		}
	}
	return 0;
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
			if(!tile->value)
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
			if(!otherTile)
				continue;
			
			VECTOR otherPos;
			mapGetVectorFromTile(map, &otherPos, otherTile);
			
			if(vec_dist(pos, &otherPos) > range)
				continue;
				
			if(fow_hasDirectLOS(map, pos, &otherPos))
				otherTile->visibility = FOW_SCOUTED;
		}
}


int fow_calcoffset = 0;
int fow_calcoffsetMAX = 16;
var fow_lightningDuration = FOW_LIGHTNING_DURATION;
void fow_update()
{
#ifdef USE_FOW
	MAP *map = mapGetCurrent();
	
	int mapSize = map->size[0]*map->size[1];
	int i;
	
	if(key_f3)
	{
		for(i = 0; i< mapSize; ++i)
		{
			TILE *tile = &((map->tiles)[i]);
			tile->visibility = FOW_SCOUTED;
		}
	}
	
	fow_lightningDuration -= time_step;
	if(fow_lightningDuration <= 0)
	{
        fow_lightningDuration = FOW_LIGHTNING_DURATION;
		fow_numLigntnings = random(3);
		for(i=0; i<fow_numLigntnings; ++i)
		{
			fow_lightningX[i] = (int)random(map->size[0]);
			fow_lightningY[i] = (int)random(map->size[1]);
		}
	}
	
	for(i = fow_calcoffset; i< mapSize; i = i+fow_calcoffsetMAX)
	{
		TILE *tile = &((map->tiles)[i]);
		if(tile->visibility == FOW_HIDDEN)
			if(fow_isTileInLOS(map, tile, FOW_SIGHT_RANGE, PLAYER_ID_PLAYER)) 
				tile->visibility = FOW_SCOUTED;
	}
	fow_calcoffset++;
	fow_calcoffset = fow_calcoffset%fow_calcoffsetMAX;
	
#endif
}
