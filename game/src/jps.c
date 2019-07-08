///////////////////////////////
// jps.c
///////////////////////////////

#include "jps.h"

///////////////////////////////

LIST* listAdd(LIST* current, void* data)
{
	LIST* new;
	
	new = (LIST*)sys_malloc(sizeof(LIST));
	new->data = data;
	new->next = current;
	
	return new;
}

LIST* listAddTileSorted(LIST* current, TILE* tile, int sortMethod)
{
	LIST* new, *first = current, *prev = NULL;
	
	new = (LIST*)sys_malloc(sizeof(LIST));
	new->data = tile;
	new->next = NULL;
	
	if(!first) return new;
	
	if(sortMethod)
	{
		if(((TILE*)first->data) == tile) return first;
		prev = first;
		LIST* current2 = first->next;
		while(current2)
		{
			TILE* tile2 = (TILE*)current2->data;
			
			if(tile2 == tile)
			{
				prev->next = current2->next;
				sys_free(current2);
				break;
			}
			prev = current2;
			current2 = current2->next;
		}
		prev = NULL;
	}
	
	while(current)
	{
		TILE* tile2 = (TILE*)current->data;
		if(tile->totalCost < tile2->totalCost)
		{
			if(prev) prev->next = new;
			else first = new;
			new->next = current;
			return first;
		}
		prev = current;
		current = current->next;
	}
	prev->next = new;
	
	return first;
}

void listDestroy(LIST* list, int mode)
{
	LIST* prev;
	
	while(list)
	{
		if(mode & (1<<0))
		{
			TILE* tile = (TILE*)list->data;
			if(tile)
			{
				if(!(tile->flags & TILE_FLAG_CURRENT_PATH)) tileResetJPSData(tile);
			}
		}
		prev = list;
		list = list->next;
		sys_free(prev);
	}
}

void draw_line2(VECTOR* vFrom, VECTOR* vTo, COLOR* color, var alpha)
{
	draw_line(vFrom,NULL,alpha);
	draw_line(vFrom,color,alpha);
	draw_line(vTo,color,alpha);
}

void draw_line3D2(VECTOR* vFrom, VECTOR* vTo, COLOR* color, var alpha)
{
	draw_line3d(vFrom,NULL,alpha);
	draw_line3d(vFrom,color,alpha);
	draw_line3d(vTo,color,alpha);
}

///////////////////////////////


UNIT* jpsAllowMovementForEntity(ENTITY* ptr, int allow)
{
	UNIT* unit = jpsUnitGetFromEntity(ptr);
	if(!unit) return NULL;
	unit->allowMovement = allow;
	return unit;
}

UNIT* jpsAllowForEntity(ENTITY* ptr, int allow)
{
	UNIT* unit = jpsUnitGetFromEntity(ptr);
	if(!unit) return NULL;
	unit->allowLogic = allow;
	return unit;
}

JPSPATH *jpsPathCreate(int maxLength)
{
	JPSPATH *jpsPath = (JPSPATH*)sys_malloc(sizeof(JPSPATH));
	memset(jpsPath, 0, sizeof(JPSPATH));
	jpsPath->maxLength = maxLength;
	jpsPath->tiles = (TILE**)sys_malloc(sizeof(TILE*) * maxLength);
	memset(jpsPath->tiles, 0, sizeof(TILE*) * maxLength);
	
	return jpsPath;
}

void jpsPathDestroy(JPSPATH *jpsPath)
{
	if(!jpsPath) return;
	if(jpsPath->tiles) sys_free(jpsPath->tiles);
	sys_free(jpsPath);
}

void mapTilesInitialize(MAP* map)
{
	int i,j,k,i2,j2;
	for(i = 0; i < map->size[0]; i++)
	{
		for(j = 0; j < map->size[1]; j++)
		{
			TILE* tile = mapTileGet(map,i,j);
			tile->pos[0] = i;
			tile->pos[1] = j;
			tile->flags = 0;
			tile->value = (!i || i == map->size[0]-1 || !j || j == map->size[1]-1);
			tile->numNeighbors = 0;
			tile->neighborList = NULL;
			tile->prev = NULL;
		}
	}
}

MAP* mapGetCurrent()
{
	if(!mapCurrent) error("mapGetCurrent: !mapCurrent");
	return mapCurrent;
}

MAP *mapCreate(int sizeX, int sizeY, VECTOR *vMin, VECTOR *vMax, var tileSize)
{
	MAP *map = (MAP*)sys_malloc(sizeof(MAP));
	memset(map, 0, sizeof(MAP));
	map->size[0] = sizeX;
	map->size[1] = sizeY;
	map->tiles = (TILE*)sys_malloc(sizeof(TILE) * map->size[0] * map->size[1]);
	mapTilesInitialize(map);
	map->bmp = bmap_createblack(map->size[0], map->size[1], 24);
	map->jpsPath = jpsPathCreate(4*sizeX);
	
	vec_set(map->vMin, vMin);
	vec_set(map->vMax, vMax);
	vec_diff(map->vSize, vMax, vMin);
	map->tileSize = tileSize;
	mapCurrent = map;
	
	return map;
}

void mapFillTest(MAP* map)
{
	int i,j,k,i2,j2;
	for(i = 0; i < map->size[0]; i++)
	{
		for(j = 0; j < map->size[1]; j++)
		{
			TILE* tile = mapTileGet(map,i,j);
			tile->pos[0] = i;
			tile->pos[1] = j;
			tile->flags = 0;
			//tile->value2 = 0; //(random(2) > 1 || !i || i == map->size[0]-1 || !j || j == map->size[1]-1);
			tile->value = (!i || i == map->size[0]-1 || !j || j == map->size[1]-1);
			tile->numNeighbors = 0;
			tile->neighborList = NULL;
			tile->prev = NULL;
		}
	}
	
	// add random solid rectangles
	int shiftX,shiftY,sizeX,sizeY;
	for(k = 0; k < 26; k++)
	{
		shiftX = random(map->size[0]);
		shiftY = random(map->size[1]);
		sizeX = 3+random(7);
		sizeY = 3+random(7);
		for(i = 0; i < sizeX; i++)
		{
			for(j = 0; j < sizeY; j++)
			{
				i2 = i+shiftX;
				j2 = j+shiftY;
				i2 %= map->size[0];
				j2 %= map->size[1];
				TILE* tile = mapTileGet(map,i2,j2);
				if(tile) tile->value = 1;
			}
		}
	}
}

void tileResetJPSData(TILE* tile)
{
	tile->currentCost = MAX_COST;
	tile->targetCost = MAX_COST;
	tile->totalCost = MAX_COST*2;
	tile->targetReachable = 0;
	tile->flags &= ~TILE_FLAG_CURRENT_PATH;
	tile->prev = NULL;
}

TILE* mapTileGet(MAP* map, int x, int y)
{
	if(x < 0 || x >= map->size[0] || y < 0 || y >= map->size[1]) return NULL;
	return &((map->tiles)[x*map->size[1] + y]);
}

void mapUpdateBmap(MAP* map)
{
	int i,j;
	BMAP* bmp = map->bmp;
	if(!bmp) return;
	var format = bmap_lock(bmp,0);
	var pixelJPS = pixel_for_vec(vector(255,0,255),100,format);
	var pixelOn = pixel_for_vec(vector(128,128,128),100,format);
	var pixelOff = pixel_for_vec(vector(0,0,0),100,format);

	for(i = 0; i < map->size[0]; i++)
	{
		for(j = 0; j < map->size[1]; j++)
		{
			TILE* tile = mapTileGet(map,i,j);
			if(tile->flags & TILE_FLAG_JPS) pixel_to_bmap(bmp,i,j,pixelJPS);
			else
			{
				if(tile->value) pixel_to_bmap(bmp,i,j,pixelOff);
				else pixel_to_bmap(bmp,i,j,pixelOn);
			}
		}	
	}
	bmap_unlock(bmp);
}

void mapTileFreeNeighborData(MAP* map, TILE* sourceTile)
{
	sourceTile->numNeighbors = 0;
	if(sourceTile->neighborList)
	{
		listDestroy(sourceTile->neighborList,0);
		sourceTile->neighborList = NULL;
	}
	if(sourceTile->neighborCost)
	{
		sys_free(sourceTile->neighborCost);
		sourceTile->neighborCost = NULL;
	}
	sourceTile->targetCost = MAX_COST;
	sourceTile->currentCost = MAX_COST;
}

void mapJPSDo(MAP* map)
{
	int i,j,k,i2,j2;
	char nearbyGrid[8];
	
	listDestroy(map->JPSTileList,0);
	map->JPSTileList = NULL;
	// to do: use previous list to free data instead of traversing the whole map
	for(i = 1; i < map->size[0]-1; i++)
	{
		for(j = 1; j < map->size[1]-1; j++)
		{
			TILE* tile = mapTileGet(map,i,j);
			
			if(tile->jpsType) mapTileFreeNeighborData(map,tile);
			tile->flags &= ~TILE_FLAG_JPS;
			tile->jpsType = 0;
		}	
	}
	int extraSpace = jpsExtraWidth;
	int range = 1+extraSpace;
	for(i = range; i < map->size[0]-range; i++)
	{
		for(j = range; j < map->size[1]-range; j++)
		{
			TILE* tile = mapTileGet(map,i,j);
			if(!tile->value)
			{
				int tileOkay = 1;
				if(extraSpace)
				{
					for(k = 0; k < 8; k++)
					{
						i2 = i+offset2D[k*2+0];
						j2 = j+offset2D[k*2+1];
						TILE* tile2 = mapTileGet(map,i2,j2);
						if(tile2)
						{
							if(tile2->value)
							{
								tileOkay = 0;
								break;
							}
						}
					}
				}
				if(tileOkay)
				{
					for(k = 0; k < 8; k++)
					{
						i2 = i+offset2D[k*2+0]*range;
						j2 = j+offset2D[k*2+1]*range;
						TILE* tile2 = mapTileGet(map,i2,j2);
						if(tile2)
						{
							if(tile2->value) nearbyGrid[k] = 1;
							else nearbyGrid[k] = 0;
						}
						else error("B");
					}
					if(!nearbyGrid[0] && !nearbyGrid[2] && nearbyGrid[4]) tile->jpsType |= (1<<4);
					if(!nearbyGrid[0] && !nearbyGrid[3] && nearbyGrid[5]) tile->jpsType |= (1<<5);
					if(!nearbyGrid[1] && !nearbyGrid[2] && nearbyGrid[7]) tile->jpsType |= (1<<6);
					if(!nearbyGrid[1] && !nearbyGrid[3] && nearbyGrid[6]) tile->jpsType |= (1<<7);
					if(tile->jpsType)
					{
						tile->flags |= TILE_FLAG_JPS;
						map->JPSTileList = listAdd(map->JPSTileList,tile);
					}
				}
			}
		}	
	}
}

//source: http://playtechs.blogspot.de/2007/03/raytracing-on-grid.html
int mapTraceDo(MAP* map, int *posFrom, int *posTo, int mode)
{
	int dx = abs(posTo[0] - posFrom[0]);
	int dy = abs(posTo[1] - posFrom[1]);
	int x = posFrom[0];
	int y = posFrom[1];
	int n = 1 + dx + dy;
	int x_inc = (posTo[0] > posFrom[0])*2-1;
	int y_inc = (posTo[1] > posFrom[1])*2-1;
	int derror = dx - dy;
	dx *= 2;
	dy *= 2;
	for (; n > 0; --n)
	{
		TILE* tile = mapTileGet(map,x,y);
		if(!tile) printf("mapTraceDo: no tile at (%d,%d)!",x,y);
		if(tile->value) return 1;

		if (derror > 0)
		{
			x += x_inc;
			derror -= dy;
		}
		else
		{
			y += y_inc;
			derror += dx;
		}
	}
	return 0;
}

var mapTileCostCalculate(MAP* map, TILE* tile1, TILE* tile2)
{
	return vec_length(vector(abs(tile1->pos[0]-tile2->pos[0]),abs(tile1->pos[1]-tile2->pos[1]),0));	
}

void mapTileCalculateVisibilityForTile(MAP* map, TILE* sourceTile)
{
	mapTileFreeNeighborData(map,sourceTile);

	LIST* listCurrent = map->JPSTileList;
	while(listCurrent)
	{
		TILE* tile = (TILE*)listCurrent->data;
		if(tile != sourceTile)
		{
			if(!mapTraceDo(map,sourceTile->pos,tile->pos,0))
			{
				sourceTile->neighborList = listAdd(sourceTile->neighborList,tile);
				sourceTile->numNeighbors++;
			}
		}
		listCurrent = listCurrent->next;
	}
	
	if(sourceTile->numNeighbors > 0)
	{
		sourceTile->neighborCost = (int*)sys_malloc(sizeof(int)*sourceTile->numNeighbors);
		int i = 0;
		listCurrent = sourceTile->neighborList;
		while(listCurrent)
		{
			TILE* tile = (TILE*)listCurrent->data;
			
			(sourceTile->neighborCost)[i] = mapTileCostCalculate(map,tile,sourceTile);
			
			i++;
			listCurrent = listCurrent->next;
		}
		if(i != sourceTile->numNeighbors) error("mapTileCalculateVisibilityForTile: mega error!");
	}
}


void mapTileCalculateVisibility(MAP* map)
{
	LIST* listCurrent;
	listCurrent = map->JPSTileList;
	while(listCurrent)
	{
		TILE* tile = (TILE*)listCurrent->data;
		mapTileCalculateVisibilityForTile(map, tile);
		listCurrent = listCurrent->next;
	}
}

//#define JPS_DEBUG

int mapJPSPathGet(MAP* map, TILE* startTile, TILE *targetTile, JPSPATH *jpsPathOut)
{
	#ifdef JPS_DEBUG
		cprintf4("\n mapJPSPathGet: START at frame %d: startTile(%p) targetTile(%p) jpsPathOut(%p)", ITF, startTile, targetTile, jpsPathOut);
	#endif
	
	if(jpsPathOut)
	{
		jpsPathOut->length = 0;
		jpsPathOut->currentNode = -1;
		jpsPathOut->startTile = startTile;
		jpsPathOut->targetTile = targetTile;
	}
	JPSPATH *jpsPath = map->jpsPath;
	int i;
	for(i = 0; i < jpsPath->length; i++)
	{
		TILE* tile = (jpsPath->tiles)[i];
		tileResetJPSData(tile);
	}
	jpsPath->length = 0;
	jpsPath->startTile = startTile;
	jpsPath->targetTile = targetTile;

	LIST* listCurrent = map->JPSTileList;
	while(listCurrent)
	{
		TILE* tile = (TILE*)listCurrent->data;
		tileResetJPSData(tile);
		listCurrent = listCurrent->next;
	}
	if(!startTile || !targetTile)
	{
		#ifdef JPS_DEBUG
			cprintf0(" EXIT(!startTile || !targetTile) ");
		#endif
		return 0;
	}
	#ifdef JPS_DEBUG
		cprintf0("A ");
	#endif
	
	if(!mapTraceDo(map,startTile->pos,targetTile->pos,0))
	{
		targetTile->currentCost = mapTileCostCalculate(map,startTile,targetTile);
		//targetTile->prev = startTile; // if we set the prev pointer here we would have to keep track of it and reset it somewhere else. better just conclude (prev == NULL) => (prev = start)
		cprintf0("->END(target visible from start)");
		return 0;
	}
	#ifdef JPS_DEBUG
		cprintf1("B(%d) ", targetTile->jpsType);
	#endif
	
	startTile->currentCost = 0;
	startTile->totalCost = 0;
	targetTile->targetCost = 0;
	targetTile->targetReachable = 1;
	targetTile->currentCost = MAX_COST;
	
	if(!targetTile->jpsType || 1) // yeah..
	{
		LIST* listCurrent = map->JPSTileList;
		while(listCurrent)
		{
			TILE* tile = (TILE*)listCurrent->data;
			if(tile != targetTile)
			{
				//tile->currentCost = MAX_COST;
				tile->targetCost = mapTileCostCalculate(map,tile,targetTile);
				tile->totalCost = tile->currentCost+tile->targetCost;
				tile->targetReachable = !mapTraceDo(map,targetTile->pos,tile->pos,0);
				#ifdef JPS_DEBUG
					if(tile->targetReachable) draw_line2(vector((targetTile.pos[0]+0.5)*16,(targetTile.pos[1]+0.5)*16,0),vector((tile->pos[0]+0.5)*16,(tile->pos[1]+0.5)*16,0),vector(0,0,255),50);
				#endif
			}
			listCurrent = listCurrent->next;
		}
	}
	#ifdef JPS_DEBUG
		cprintf1("C(%d) ", startTile->jpsType);
	#endif
	
	LIST* listActive = NULL;
	if(!startTile->jpsType)
	{
		LIST* listCurrent = map->JPSTileList;
		while(listCurrent)
		{
			TILE* tile = (TILE*)listCurrent->data;
			
			if(!mapTraceDo(map,startTile->pos,tile->pos,0))
			{
				tile->currentCost = mapTileCostCalculate(map,tile,startTile);
				tile->totalCost = tile->currentCost+tile->targetCost;
				tile->prev = startTile;
				listActive = listAddTileSorted(listActive,tile,0);
				#ifdef JPS_DEBUG
					draw_line2(vector((startTile.pos[0]+0.5)*16,(startTile.pos[1]+0.5)*16,0),vector((tile->pos[0]+0.5)*16,(tile->pos[1]+0.5)*16,0),vector(0,255,0),50);
				#endif
			}
			
			listCurrent = listCurrent->next;
		}
	}
	else listActive = listAdd(NULL,startTile);
	
	
	#ifdef JPS_DEBUG
		cprintf0("D ");
	#endif
	// actual search
	while(listActive)
	{
		TILE* tile = (TILE*)listActive->data;
		LIST *prev = listActive;
		listActive = listActive->next;
		sys_free(prev);
		
		if(tile == targetTile) break;
		if(tile->targetReachable) // && !key_f)
		{
			int newCost = tile->currentCost+tile->targetCost;
			if(newCost < targetTile->currentCost)
			{
				targetTile->prev = tile;
				targetTile->currentCost = newCost;
				targetTile->totalCost = targetTile->currentCost;
				listActive = listAddTileSorted(listActive,targetTile,1);
			}
		}
		int i = 0;
		LIST* neighborList = tile->neighborList;
		while(neighborList)
		{
			
			TILE* tile2 = (TILE*)neighborList->data;
			
			int newCost = tile->currentCost+(tile->neighborCost)[i];
			if(newCost < tile2->currentCost)
			{
				tile2->prev = tile;
				tile2->currentCost = newCost;
				tile2->totalCost = tile2->currentCost+tile2->targetCost;
				listActive = listAddTileSorted(listActive,tile2,1);
			}
			/* may be too early in some cases
			if(tile2 == targetTile) break; */
			i++;
			neighborList = neighborList->next;
		}
	}
	#ifdef JPS_DEBUG
		cprintf0("E ");
	#endif
	
	sys_marker("JE1");
	TILE* current = targetTile;
	while(current && jpsPath->length < jpsPath->maxLength)
	{
		current->flags |= TILE_FLAG_CURRENT_PATH;
		sys_marker("JE2");
		(jpsPath->tiles)[jpsPath->length++] = current;
		if(jpsPathOut)
		{
			sys_marker("JE3");
			if(jpsPathOut->length < jpsPathOut->maxLength) (jpsPathOut->tiles)[jpsPathOut->length++] = current;
		}
		sys_marker("JE4");
		if(current == startTile) break; // there was a case where startTile->prev could be != NULL, an issue with the code, *could* be fixed now
		current = current->prev;
	}
	sys_marker(NULL);
	#ifdef JPS_DEBUG
		cprintf0("E1 ");
	#endif
	if(jpsPathOut) jpsPathOut->currentNode = jpsPathOut->length - 1;
	#ifdef JPS_DEBUG
		cprintf0("E2 ");
	#endif

	listDestroy(listActive, (1<<0));
	#ifdef JPS_DEBUG
		cprintf0("E3 ");
	#endif
	//dJPSTime = dtimer();
	#ifdef JPS_DEBUG
		cprintf0("->END(end of function) ");
	#endif
	
	return jpsPath->length;
}

void mapJPSUpdate(MAP* map)
{
	// calculate jump points
	mapJPSDo(map);
	// create jump point to jump point neighbor lists
	mapTileCalculateVisibility(map);
}


void presetsInit()
{
	// 같같같같같같� UNIT_SPUTNIK 같같같같같같�
	UNIT_PRESET* unitPreset = &unitPresets[UNIT_SPUTNIK];
	//strcpy(unitPreset->file, "warlock.mdl");
	unitPreset->ID = UNIT_SPUTNIK;
	unitPreset->maxSpeed = 0.075;
	unitPreset->maxHP = 100;
	unitPreset->radius = 0.15;
	unitPreset->armorType = ARMOR_TYPE_HUMANS;
	
	PROJECTILE_PRESET* projectilePreset = &projectilePresets[UNIT_SPUTNIK];
	projectilePreset->ID = UNIT_SPUTNIK;
	projectilePreset->damage[ARMOR_TYPE_HUMANS] = 12; projectilePreset->damage[ARMOR_TYPE_VEHICLE_LIGHT] = 12; projectilePreset->damage[ARMOR_TYPE_VEHICLE_HEAVY] = 12;
	projectilePreset->range = 2;
	projectilePreset->cooldown = 14; // in ticks
	projectilePreset->speed = 0.9;
	
	// 같같같같같같� UNIT_LERCHE 같같같같같같�
	UNIT_PRESET* unitPreset = &unitPresets[UNIT_LERCHE];
	unitPreset->ID = UNIT_LERCHE;
	unitPreset->maxSpeed = 0.075;
	unitPreset->maxHP = 100;
	unitPreset->radius = 0.15;
	unitPreset->armorType = ARMOR_TYPE_HUMANS;
	
	PROJECTILE_PRESET* projectilePreset = &projectilePresets[UNIT_LERCHE];
	projectilePreset->ID = UNIT_LERCHE;
	projectilePreset->damage[ARMOR_TYPE_HUMANS] = 12; projectilePreset->damage[ARMOR_TYPE_VEHICLE_LIGHT] = 50; projectilePreset->damage[ARMOR_TYPE_VEHICLE_HEAVY] = 50;
	projectilePreset->range = 3;
	projectilePreset->cooldown = 16; // in ticks
	projectilePreset->speed = 0.45;
	
	// 같같같같같같� UNIT_EYE 같같같같같같�
	UNIT_PRESET* unitPreset = &unitPresets[UNIT_EYE];
	unitPreset->ID = UNIT_EYE;
	unitPreset->maxSpeed = 0.125;
	unitPreset->maxHP = 100;
	unitPreset->radius = 0.175;
	unitPreset->armorType = ARMOR_TYPE_HUMANS;
	
	PROJECTILE_PRESET* projectilePreset = &projectilePresets[UNIT_EYE];
	projectilePreset->ID = UNIT_EYE;
	projectilePreset->range = -1;
	
	// 같같같같같같� UNIT_BABE 같같같같같같�
	UNIT_PRESET* unitPreset = &unitPresets[UNIT_BABE];
	unitPreset->ID = UNIT_BABE;
	unitPreset->maxSpeed = 0.175;
	unitPreset->maxHP = 200;
	unitPreset->radius = 0.225;
	unitPreset->armorType = ARMOR_TYPE_VEHICLE_LIGHT;
	
	PROJECTILE_PRESET* projectilePreset = &projectilePresets[UNIT_BABE];
	projectilePreset->ID = UNIT_BABE;
	projectilePreset->damage[ARMOR_TYPE_HUMANS] = 12; projectilePreset->damage[ARMOR_TYPE_VEHICLE_LIGHT] = 12; projectilePreset->damage[ARMOR_TYPE_VEHICLE_HEAVY] = 1;
	projectilePreset->range = 2.25;
	projectilePreset->cooldown = 2; // in ticks
	projectilePreset->speed = 1;
	
	// 같같같같같같� UNIT_SKULL 같같같같같같�
	UNIT_PRESET* unitPreset = &unitPresets[UNIT_SKULL];
	unitPreset->ID = UNIT_SKULL;
	unitPreset->maxSpeed = 0.0875;
	unitPreset->maxHP = 400;
	unitPreset->radius = 0.225;
	unitPreset->armorType = ARMOR_TYPE_VEHICLE_HEAVY;
	
	PROJECTILE_PRESET* projectilePreset = &projectilePresets[UNIT_SKULL];
	projectilePreset->ID = UNIT_SKULL;
	projectilePreset->damage[ARMOR_TYPE_HUMANS] = 10; projectilePreset->damage[ARMOR_TYPE_VEHICLE_LIGHT] = 80; projectilePreset->damage[ARMOR_TYPE_VEHICLE_HEAVY] = 50;
	projectilePreset->range = 5;
	projectilePreset->cooldown = 24; // in ticks
	projectilePreset->speed = 0.5;
	
}

VECTOR* mapGetVectorFromTile(MAP* map, VECTOR* v, TILE* tile)
{
	if(!tile) error("you called mapGetVectorFromTile with a NULL tile, probably because of mapGetTile and bad indices!");
	static VECTOR _vstatic;
	if(!v) v = &_vstatic;
	v->x = (tile->pos[0]+0.5)*map->tileSize + map->vMin.x;
	v->y = (tile->pos[1]+0.5)*map->tileSize + map->vMin.y;
	v->z = 0.5*map->tileSize;
	return v;
}

VECTOR* mapGetVector2DFromVector3D(MAP* map, VECTOR* v2d, VECTOR* v3d)
{
	static VECTOR _vstatic;
	if(!v2d) v2d = &_vstatic;
	v2d->x = (v3d->x-map->vMin.x)/map->tileSize;
	v2d->y = (v3d->y-map->vMin.y)/map->tileSize;
	v2d->z = 0;
	return v2d;
}


VECTOR* mapGetVector3DFromVector2D(MAP* map, VECTOR* v3d, VECTOR* v2d)
{
	static VECTOR _vstatic;
	if(!v3d) v3d = &_vstatic;
	v3d->x = v2d->x*map->tileSize + map->vMin.x;
	v3d->y = v2d->y*map->tileSize + map->vMin.y;
	//v3d->z = 0; // do not overwrite height
	return v3d;
}

TILE* mapGetTileFromVector(MAP* map, VECTOR* v)
{
	int i = (v->x-map->vMin.x)/map->tileSize;
	int j = (v->y-map->vMin.y)/map->tileSize;
	return mapTileGet(map, i, j);
}

void drawStr3D(STRING* str, VECTOR* pos3d, COLOR *color)
{
	VECTOR temp;
	vec_set(temp, pos3d);
	if(vec_to_screen(temp,camera))
	{
		draw_text(str, temp.x-9,temp.y-9,COLOR_BLACK);
		draw_text(str, temp.x-10,temp.y-10,color);
	}
}

void draw_text2(STRING* str, var x, var y, COLOR *color)
{
	draw_text(str, x+1,y+1,COLOR_BLACK);
	draw_text(str, x,y,color);
}

void jpsUnitDestroySimple(UNIT* unit)
{
	if(unit->jpsPath) jpsPathDestroy(unit->jpsPath);
	sys_free(unit);	
}

void mapUpdateUnits(MAP* map)
{
	int currentPlayer;
	int i,j;
	for(i = 1; i < map->size[0]-1; i++)
	{
		for(j = 1; j < map->size[1]-1; j++)
		{
			TILE* tile = mapTileGet(map,i,j);
			/*if(tile->unitList)
			{
				listDestroy(tile->unitList, 0);
				tile->unitList = NULL;
			}*/
			for(currentPlayer = 0; currentPlayer < MAX_PLAYERS; currentPlayer++) tile->numUnits[currentPlayer] = 0;
		}
	}
	
	for(currentPlayer = 0; currentPlayer < MAX_PLAYERS; currentPlayer++)
	{
		UNIT* prev = NULL;
		UNIT *unit = map->unitFirst[currentPlayer];
		while(unit)
		{
			UNIT* next = unit->next;
			if(unit->HP <= 0) unit->flaggedForDeletion++;
			if(unit->flaggedForDeletion >= 2)
			{
				map->numUnits[unit->playerID]--;
				if(prev) prev->next = next;
				else map->unitFirst[currentPlayer] = next;
				jpsUnitDestroySimple(unit);
				unit->ent->SK_ENTITY_JPS_POINTER_TO_UNIT_STRUCT = NULL;
			}
			else
			{
				if(unit->victimUnit)
				{
					if(unit->victimUnit->flaggedForDeletion) unit->victimUnit = NULL;
				}
				prev = unit;
				TILE* tile = mapTileGet(map, unit->pos2d.x, unit->pos2d.y);
				if(tile)
				{
					if(tile->numUnits[currentPlayer] < MAX_UNITS_PER_TILE)
					{
						tile->unitArray[currentPlayer][tile->numUnits[currentPlayer]] = unit;
						tile->numUnits[currentPlayer]++;
					}
				}
				if(unit->tile) unit->prevTile = unit->tile;
				unit->tile = tile;
			}
			unit = next;
		}
	}
	/*for(i = 1; i < map->size[0]-1; i++)
	{
		for(j = 1; j < map->size[1]-1; j++)
		{
			TILE* tile = mapTileGet(map,i,j);
			VECTOR temp;
			mapGetVector3DFromVector2D(map, temp, vector(i+0.5,j+0.5,0));
			temp.z = 0;
			if(vec_to_screen(temp,camera))
			{
				draw_text2( str_printf(NULL, "%d", (int)tile->numUnits[0]), temp.x-10, temp.y-10, COLOR_WHITE);
				draw_text2( str_printf(NULL, "%d", (int)tile->numUnits[1]), temp.x-10, temp.y+10, COLOR_WHITE);
			}
		}
	}*/
	
}

int mapGetNearbyUnits(MAP* map, TILE* sourceTile, int range)
{
	pointerArrayNum = 0;
	if(!sourceTile) return NULL;
	int i,j;
	for(i = sourceTile->pos[0]-range; i <= sourceTile->pos[0]+range; i++)
	{
		for(j = sourceTile->pos[1]-range; j <= sourceTile->pos[1]+range; j++)
		{
			TILE* tile = mapTileGet(map, i, j);
			if(tile)
			{
				int k;
				int currentPlayer;
				for(currentPlayer = 0; currentPlayer<MAX_PLAYERS; ++currentPlayer)
				{		
					for(k = 0; k < tile->numUnits[currentPlayer]; k++)
					{
						pointerArray[pointerArrayNum++] = tile->unitArray[currentPlayer][k];
						if(pointerArrayNum >= POINTER_ARRAY_MAX) return pointerArrayNum;
					}
				}
			}
		}
	}
	return pointerArrayNum;
}

int unit_getType(ENTITY* ent);

// set typeID to -1 to get all objects
// owner to -1 for both player and enemy units
int mapGetNearbyUnitsOfTypeForPos(VECTOR *vpos, int typeID, int owner, var maxDistance, int maxNumEntities)
{
	MAP* map = mapGetCurrent();
	TILE* sourceTile = mapGetTileFromVector(map, vpos);
	pointerArrayNum = 0;
	if(!sourceTile) return 0;
	int range = 1+maxDistance/map->tileSize;
	int ownerMin = 0;
	int ownerMax = 1;
	int currentPlayer;
	
	if(owner == PLAYER_ID_PLAYER) currentPlayer = 0; 
	if(owner == PLAYER_ID_AI) currentPlayer = 1; 
	
	int i,j;
	for(i = sourceTile->pos[0]-range; i <= sourceTile->pos[0]+range; i++)
	{
		for(j = sourceTile->pos[1]-range; j <= sourceTile->pos[1]+range; j++)
		{
			TILE* tile = mapTileGet(map, i, j);
			if(tile)
			{
				if (fow_isVisible(tile))
				//if (tile->visibility == FOW_SCOUTED)
				{
					int k;
					for(k = 0; k < tile->numUnits[currentPlayer]; k++)
					{
						UNIT* unit = tile->unitArray[currentPlayer][k];
						ENTITY* ent = unit->ent;
						if(ent)
						{
							//removing units from jps either does not work or happens too late... filter dead units
							if((typeID < 0 || typeID == unit_getType(ent)) && ent->HEALTH > 0 /* dirty hack */)
							{
								pointerArray[pointerArrayNum++] = tile->unitArray[currentPlayer][k];
								if(pointerArrayNum >= POINTER_ARRAY_MAX || pointerArrayNum >= maxNumEntities) return pointerArrayNum;
							}
						}
					}
				}
			}
		}
	}
	return pointerArrayNum;
}


int mapIsAnyUnitNearby(MAP* map, TILE* sourceTile, int range)
{
	if(!sourceTile) return NULL;
	int i,j;
	for(i = sourceTile->pos[0]-range; i <= sourceTile->pos[0]+range; i++)
	{
		for(j = sourceTile->pos[1]-range; j <= sourceTile->pos[1]+range; j++)
		{
			TILE* tile = mapTileGet(map, i, j);
			int currentPlayer;
			if(tile)
			for(currentPlayer = 0; currentPlayer<MAX_PLAYERS; ++currentPlayer)	
			if(tile->numUnits[currentPlayer]) return 1;
		}
	}
	return 0;
}


int mapIsAnyFriendlyUnitNearby(MAP* map, TILE* sourceTile, int range, int playerNumber)
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
			if(tile) {
				VECTOR otherPos;
				mapGetVectorFromTile(map, &otherPos, tile);
				
				if(vec_dist(pos, &otherPos) > range)
				continue;
				if(tile->numUnits[playerNumber]) return 1;
			}
		}
	}
	return 0;
}

VECTOR* unitFlockingSpeedGet(MAP* map, UNIT* unit, VECTOR* v)
{
	static VECTOR _vstatic;
	if(!v) v = &_vstatic;
	
	UNIT_PRESET* unitPreset = &unitPresets[unit->presetID];
	vec_set(v, nullvector);
	//LIST* unitList = mapGetNearbyUnits(map, unit->tile, 2);
	//while(unitList)
	//UNIT* neighbor = (UNIT*)unitList->data;
	int numNeighbors = mapGetNearbyUnits(map, unit->tile, 1);
	int i;
	for(i = 0; i < numNeighbors; i++)
	{
		UNIT* neighbor = (UNIT*)pointerArray[i];
		int size = 12;
		int unitDrawSize = 2;
		//draw_line2(vector(unit->pos2d.x*size-unitDrawSize/2,unit->pos2d.y*size-unitDrawSize/2,0), vector(neighbor->pos2d.x*size-unitDrawSize/2,neighbor->pos2d.y*size-unitDrawSize/2,0), vector(255,128,64), 100);
		
		UNIT_PRESET* neighborPreset = &unitPresets[neighbor->presetID];
		VECTOR vDir;
		vec_diff(vDir, unit->pos2d, neighbor->pos2d);
		var length = vec_length(vDir);
		var range = 2*(unitPreset->radius + neighborPreset->radius);
		
		if(length < range)
		{
			vec_normalize(vDir, (range-length)/range);
			vec_add(v, vDir);
		}
		//unitList = unitList->next;
	}
	//if(unitList) listDestroy(unitList, 0);
	
	return v;
}

int sphereBoxCollision(VECTOR* spherePos2D, VECTOR* vSpeed, float sphereRadius, float boxMinX, float boxMinY, float boxMaxX, float boxMaxY)
{
	VECTOR pbox;
	pbox.z = 0;
	//pbox.x = clamp(spherePos2D->x, boxMinX, boxMaxX);
	//pbox.y = clamp(spherePos2D->y, boxMinY, boxMaxY);
	float midX = (boxMinX+boxMaxX)*0.5;
	float midY = (boxMinY+boxMaxY)*0.5;
	VECTOR vMid;
	vMid.x = midX;
	vMid.y = midY;
	vMid.z = 0;
	
	/*pbox.x = clamp(spherePos2D->x, boxMinX, boxMaxX);
	pbox.y = clamp(spherePos2D->y, boxMinY, boxMaxY);
	pbox.z = 0;
	int caseA = (pbox.x == spherePos2D->x);
	int caseB = (pbox.y == spherePos2D->y);*/
	
	float tolerance = 0; //0.01;
	int caseA = 0;
	int caseB = 0;
	if(spherePos2D->x < boxMinX) pbox.x = boxMinX-tolerance;
	else
	{
		if(spherePos2D->x > boxMaxX) pbox.x = boxMaxX+tolerance;
		else
		{
			pbox.x = spherePos2D->x;
			caseA = 1;
		}
	}
	if(spherePos2D->y < boxMinY) pbox.y = boxMinY-tolerance;
	else
	{
		if(spherePos2D->y > boxMaxY) pbox.y = boxMaxY+tolerance;
		else
		{
			pbox.y = spherePos2D->y;
			caseB = 1;
		}
	}
	int sphereIsInBox = caseA*caseB;
	if(sphereIsInBox)
	{
		if(spherePos2D->x < midX) pbox.x = boxMinX;
		else pbox.x = boxMaxX;
		if(spherePos2D->y < midY) pbox.y = boxMinY;
		else pbox.y = boxMaxY;
	}
	int size = 12;
	int unitDrawSize = 2;
	if(sphereIsInBox)
	{
		unitDrawSize *= 2;
		//draw_quad(NULL,vector(pbox.x*size-unitDrawSize/2,pbox.y*size-unitDrawSize/2,0),NULL,vector(unitDrawSize,unitDrawSize,0),NULL,vector(255,255,0),100,0);
		vec_set(spherePos2D, pbox);
		//return 0;
		/*
		VECTOR vDir;
		vec_diff(vDir, spherePos2D, vMid);
		if(abs(vDir.x) > abs(vDir.y))
		{
			if(spherePos2D->x < midX) spherePos2D.x = boxMinX;
			else spherePos2D.x = boxMaxX;
		}
		else
		{
			if(spherePos2D->y < midY) spherePos2D.y = boxMinY;
			else spherePos2D.y = boxMaxY;
		}
		return 1;
		//vec_normalize(vDir, maxv( minv(abs(vDir.x),abs(vDir.y)), );
		*/
	}
	//else
	{
		//draw_quad(NULL,vector(pbox.x*size-unitDrawSize/2,pbox.y*size-unitDrawSize/2,0),NULL,vector(unitDrawSize,unitDrawSize,0),NULL,vector(255,0,255),100,0);
		VECTOR delta;
		vec_diff(delta, spherePos2D, pbox);
		float len = vec_length(delta);
		if(len < 0.025) vec_sub(delta,vSpeed);
		if(len < sphereRadius)
		{
			/*VECTOR psphere;
			vec_diff(psphere, pbox, spherePos2D);
			vec_normalize(psphere, sphereRadius);
			vec_add(psphere, spherePos2D);*/
			//int unitDrawSize = 2;
			//draw_quad(NULL,vector(psphere.x*size-unitDrawSize/2,psphere.y*size-unitDrawSize/2,0),NULL,vector(unitDrawSize,unitDrawSize,0),NULL,vector(0,255,255),100,0);
			VECTOR vCorrection;
			vec_set(vCorrection, delta);
			vec_normalize(vCorrection, sphereRadius-len); //(sphereRadius-len)/len);
			vec_add(spherePos2D, vCorrection);
			//vec_scale(delta, vec_dot(vSpeed, delta)/ (len*len));
			//vec_sub(vSpeed, delta);
			return 1;
		}
	}
	return 0;
}

int unitMoveWithCollision(MAP* map, UNIT* unit, VECTOR *vSpeed)
{
	if(!unit->tile)
	{
		vec_add(unit->pos2d, vSpeed);
		return -1;
	}
	VECTOR prevPos;
	vec_set(prevPos, unit->pos2d);
	vec_add(unit->pos2d, vSpeed);
	//VECTOR vNewPos;
	//vec_set(vNewPos, unit->pos2d);
	//vec_add(vNewPos, vSpeed);
	int hitTrue = 0;
	UNIT_PRESET* unitPreset = &unitPresets[unit->presetID];
	TILE* sourceTile = unit->tile;
	int i,j;
	/*TILE* tiles[3][3];
	int 
	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < 3; j++)
		{
			TILE* tile = mapTileGet(map, sourceTile->pos[0]+i-1, sourceTile->pos[1]+j-1);
			if(tile)
		}
	}*/
	
	for(i = sourceTile->pos[0]-1; i <= sourceTile->pos[0]+1; i++)
	{
		for(j = sourceTile->pos[1]-1; j <= sourceTile->pos[1]+1; j++)
		{
			if(1) //i != sourceTile->pos[0] || j != sourceTile->pos[1])
			{
				TILE* tile = mapTileGet(map, i, j);
				if(tile)
				{
					if(tile->value)
					{
						int size = 12;
						int unitDrawSize = 2;
						//VECTOR vCorrection;
						if(sphereBoxCollision(unit->pos2d, vSpeed, unitPreset->radius, i, j, (i+1), (j+1))) //, vCorrection)) // *map->tileSize
						{
							//vec_normalize(vCorrection,1);
							//vec_scale(vCorrection, vec_dot(vCorrection, vSpeed)*1.05);
							//vec_sub(vSpeed, vCorrection); 
							//vec_add(vSpeed, vCorrection);
							hitTrue = 1;
							////draw_line2(vector(prevPos.x*size-unitDrawSize/2,prevPos.y*size-unitDrawSize/2,0), vector((tile->pos[0]+0.5)*size,(tile->pos[1]+0.5)*size,0), vector(64,64,255), 50);
						}
						//else draw_line2(vector(prevPos.x*size-unitDrawSize/2,prevPos.y*size-unitDrawSize/2,0), vector((tile->pos[0]+0.5)*size,(tile->pos[1]+0.5)*size,0), vector(64,255,64), 50);
					}
				}
			}
		}
	}
	//vec_normalize(vSpeed, 0.5*time_step);
	//vec_add(unit->pos2d, vSpeed);
	return hitTrue;
}

void unitMove(MAP* map, UNIT* unit)
{
	//if(!key_q) return;

	UNIT_PRESET* unitPreset = &unitPresets[unit->presetID];
	float maxSpeed = unitPreset->maxSpeed;
	VECTOR vDir, currentTarget;
	if(!unit->targetReached)
	{
		if(vec_dist(unit->target2d, unit->pos2d) < 0.35) unit->targetReached = 1;
	}
	if(unit->targetReached)
	{
		//vec_set(currentTarget, unit->pos2d);
		vec_set(vDir,nullvector);
		vec_set(unit->currentTarget2d, unit->pos2d);
	}
	else
	{
		vec_set(currentTarget, unit->target2d);
		JPSPATH *jpsPath = unit->jpsPath;
		if(jpsPath && unit->tile)
		{
			if(jpsPath->currentNode >= 0 && jpsPath->length > 0) //jpsPath->length)
			{
				TILE* unitTile = unit->tile;
				TILE* nextTile = (jpsPath->tiles)[jpsPath->currentNode];
				if(!nextTile) printf("ERROR: !nextTile in unitMove(%d), path(%p, %d/%d)",unit->uniqueID, jpsPath, (int)jpsPath->currentNode, (int)jpsPath->length);
				//if(!mapTraceDo(map,unitTile->pos,nextTile->pos,0))
				if(vec_length( vector(unitTile->pos[0]-nextTile->pos[0], unitTile->pos[1]-nextTile->pos[1], 0) ) < 0.25 )
				{
					jpsPath->currentNode--;
					if(jpsPath->currentNode >= 0) nextTile = (jpsPath->tiles)[jpsPath->currentNode];
					else nextTile = NULL;
				}
				if(nextTile) vec_set(currentTarget, vector(nextTile->pos[0]+0.5, nextTile->pos[1]+0.5, 0));
			}
		}
		vec_set(unit->currentTarget2d,currentTarget);
		int size = 12;
		//draw_line2(vector(unit->pos2d.x*size,unit->pos2d.y*size,0), vector(currentTarget.x*size,currentTarget.y*size,0), vector(0,255,255), 50);
		vec_diff(vDir, currentTarget, unit->pos2d);
		vDir.z = 0;
		var length = vec_length(vDir);
		if(length > maxSpeed) vec_normalize(vDir, maxSpeed);
		VECTOR vDirOverwrite;
		vec_set(vDirOverwrite, vector(unit->speedOverwrite.x*maxSpeed, unit->speedOverwrite.y*maxSpeed, 0));
		vec_lerp(vDir, vDir, vDirOverwrite, unit->speedOverwrite.z);
	}
	VECTOR vFlock;
	unitFlockingSpeedGet(map, unit, &vFlock);
	vFlock.z = 0;
	//vec_scale(vFlock, 0.1);
	var length = vec_length(vFlock);
	if(length > maxSpeed*4) vec_normalize(vFlock, maxSpeed*4);
	vec_add(vDir, vFlock);
	var length = vec_length(vDir);
	if(length > maxSpeed) vec_normalize(vDir, maxSpeed);
	vec_scale(vDir, unit->speedFac*time_step);
	if(vDir.z) error("unitMove: vDir.z");

	/*int moveWithCollision = 0;
	if(unit->tile)
	{
		// ...
		if(!unit->tile->value) moveWithCollision = 1;
	}
	if(moveWithCollision) unitMoveWithCollision(map, unit, vDir);
	else vec_add(unit->pos2d, vDir);*/
	unitMoveWithCollision(map, unit, vDir);
}


void jpsUnitSetEnemyTargetUnit(MAP* map, UNIT* unit, UNIT* victimUnit)
{
	unit->victimUnit = victimUnit;
	unitSetTargetFromVector2D(map, unit, victimUnit->pos2d);
}

/*int mapGetScanForNearbyEnemyUnits(MAP* map, VECTOR *vPos2d, int range)
{
	pointerArrayNum = 0;
	MAP* map = mapGetCurrent();
	TILE* sourceTile = mapTileGet(map, vPos2d.x, vPos2d.y);
	if(!sourceTile) return 0;
	int i,j;
	for(i = sourceTile->pos[0]-range; i <= sourceTile->pos[0]+range; i++)
	{
		for(j = sourceTile->pos[1]-range; j <= sourceTile->pos[1]+range; j++)
		{
			TILE* tile = mapTileGet(map, i, j);
			if(tile)
			{
				int k;
				int currentPlayer;
				for(currentPlayer = 0; currentPlayer<MAX_PLAYERS; ++currentPlayer)
				{		
					for(k = 0; k < tile->numUnits[currentPlayer]; k++)
					{
						pointerArray[pointerArrayNum++] = tile->unitArray[k];
						if(pointerArrayNum >= POINTER_ARRAY_MAX) return pointerArrayNum;
					}
				}
			}
		}
	}
	return pointerArrayNum;
}*/

int ceil(var x)
{
	return -floor(-x);
}

var vec_dist2d(VECTOR* v1, VECTOR* v2)
{
	return vec_length(vector(v1->x-v2->x,v1->y-v2->y,0));
}

UNIT* mapScanForVictimUnit(MAP* map, UNIT* unit)
{
	TILE* sourceTile = mapTileGet(map, unit->pos2d.x, unit->pos2d.y);
	if(!sourceTile) return NULL;

	PROJECTILE_PRESET* projectilePreset = &projectilePresets[unit->presetID];
	int opposingPlayer = !unit->playerID;
	int range = ceil(projectilePreset->range);
	pointerArrayNum = 0;
	int i,j;
	for(i = sourceTile->pos[0]-range; i <= sourceTile->pos[0]+range; i++)
	{
		for(j = sourceTile->pos[1]-range; j <= sourceTile->pos[1]+range; j++)
		{
			TILE* tile = mapTileGet(map, i, j);
			if(tile)
			{
				int k;
				for(k = 0; k < tile->numUnits[opposingPlayer]; k++)
				{
					pointerArray[pointerArrayNum++] = tile->unitArray[opposingPlayer][k];
					if(pointerArrayNum >= POINTER_ARRAY_MAX)
					{
						break;
					}
				}
			}
		}
	}
	//if(unit->playerID == PLAYER_ID_PLAYER) cprintf3("\n mapScanForVictimUnit(uID %d) at frame %d: pointerArrayNum(%d)", unit->uniqueID, (int)total_frames, pointerArrayNum);

	var minDist = projectilePreset->range*1.5;
	UNIT* finalVictim = NULL;
	for(i = 0; i < pointerArrayNum; i++)
	{
		UNIT* possibleVictim = pointerArray[i];
		var dist = vec_dist2d(unit->pos2d, possibleVictim->pos2d);
		if(dist < minDist)
		{
			minDist = dist;
			finalVictim = possibleVictim;
		}
	}
	//if(unit->playerID == PLAYER_ID_PLAYER) cprintf1(" ->minDist(%.2f)", (double)minDist);
	return finalVictim;
}

void projectileSpawnForUnitAndVictim(MAP* map, int presetID, UNIT* unit, UNIT* victim)
{
	int playerID = unit->playerID;
	if(map->numProjectiles[playerID] >= MAX_PROJECTILES) return;
	PROJECTILE *projectile = &(map->projectiles[playerID][map->numProjectiles[playerID]]);
	projectile->presetID = presetID;
	projectile->playerID = playerID;
	projectile->lifetime = 32;
	projectile->victim = victim;
	vec_set(projectile->pos2d, unit->pos2d);
	vec_set(projectile->prevPos2d, unit->pos2d);
	map->numProjectiles[playerID]++;
}

void unitDoStep(MAP* map, UNIT* unit)
{
	if(unit->attackCooldown > 0)
	{
		unit->attackCooldown = maxv(unit->attackCooldown-time_step,0);
		return;	
	}	
	UNIT* enemy = unit->victimUnit;
	UNIT_PRESET* unitPreset = &unitPresets[unit->presetID];
	PROJECTILE_PRESET* projectilePreset = &projectilePresets[unit->presetID];
	if(enemy)
	{
		unitSetTargetFromVector2D(map, unit, enemy->pos2d);

		if(vec_dist(unit->pos2d, enemy->pos2d) < projectilePreset->range)
		{
			unit->attackCooldown = projectilePreset->cooldown;
			projectileSpawnForUnitAndVictim(map, unit->presetID, unit, enemy);
			return;
		}
	}
	else
	{
		if(projectilePreset->range > 0 && (unit->attackMove || unit->targetReached))
		{
			if(!((unit->uniqueID+total_frames)%20))
			{
				unit->victimUnit = mapScanForVictimUnit(map, unit);
			}
		}
	}
	unitMove(map, unit);
}

void mapMoveUnits(MAP* map)
{
	int currentPlayer;
	for(currentPlayer = 0; currentPlayer < MAX_PLAYERS; currentPlayer++)
	{
		UNIT* prev = NULL;
		UNIT *unit = map->unitFirst[currentPlayer];
		while(unit)
		{
			UNIT* next = unit->next;
			UNIT_PRESET* unitPreset = &unitPresets[unit->presetID];
			vec_set(unit->prevPos3d, unit->pos3d);
			if(unit->allowLogic) //unit->isActive)
			{
				unitDoStep(map, unit);
				mapGetVector3DFromVector2D(map, unit->pos3d, unit->pos2d);
				mapGetVector3DFromVector2D(map, unit->currentTarget3d, unit->currentTarget2d);
				mapGetVector3DFromVector2D(map, unit->target3d, unit->target2d);
				//if(unit->ent) mapGetVector3DFromVector2D(map, unit->ent.x, unit->pos2d);
				prev = unit;
			}
			else
			{
				unit->isMoving = 0;
			}
			/*else
			{
				if(prev) prev->next = next;
				else map->unitFirst[currentPlayer] = next;
				sys_free(unit);
			}*/
			unit = next;
		}
	}
}

void p_lerche_proj_trail_fade(PARTICLE* p)
{
	vec_scale(p.vel_x,1-0.15*time_step);
	if(p.lifespan < 8)
	{
		p.size = p.skill_a*p.lifespan/8.0;
	}
}

void p_lerche_proj_trail(PARTICLE* p)
{
	vec_normalize(p.vel_x,8);
	p.vel_x += (1-random(2))*2;
	p.vel_y += (1-random(2))*2;
	p.vel_z += (1-random(2))*2;
	set(p,MOVE);
	p.size = p.skill_a = 3*(random(3)+13);
	p.lifespan = random(2)+10;
	p.alpha = 75+random(10);
	p.event = p_lerche_proj_trail_fade;
}


void p_skull_proj_impact_fade(PARTICLE* p)
{
	p.vel_x *= 1-0.15*time_step;
	p.vel_y *= 1-0.15*time_step;
	p.vel_z -= 8*time_step;
	if(p.lifespan < 4)
	{
		p.size = p.skill_a*p.lifespan/4.0;
	}
}

void p_skull_proj_impact(PARTICLE* p)
{
	vec_set(p.blue,vector(0,255,255));
	vec_set(p.vel_x,vector((1-random(2))*2,(1-random(2))*2,random(2)));
	vec_normalize(p.vel_x,30+random(15));
	p.vel_z += 30+random(30);
	set(p,MOVE);
	p.size = p.skill_a = random(9)+14;
	p.lifespan = random(2)+12;
	p.alpha = 100;
	p.event = p_skull_proj_impact_fade;
}


void mapUpdateProjectiles(MAP* map)
{
	VECTOR dir;
	int currentPlayer, currentProjectile;
	for(currentPlayer = 0; currentPlayer < MAX_PLAYERS; currentPlayer++)
	{
		for(currentProjectile = 0; currentProjectile < map->numProjectiles[currentPlayer]; currentProjectile++)
		{
			PROJECTILE *projectile = &(map->projectiles[currentPlayer][currentProjectile]);
			PROJECTILE_PRESET* projectilePreset = &projectilePresets[projectile->presetID];
			vec_set(projectile->prevPos2d, projectile->pos2d);
			UNIT *victim = projectile->victim;
			if(victim)
			{
				if(victim->flaggedForDeletion)
				{
					victim = NULL;
					projectile->victim = NULL;
				}
			}
			if(victim) vec_set(projectile->target2d, victim->pos2d);
			
			int projectileHitTrue = 0;
			vec_diff(dir, projectile->target2d, projectile->pos2d);
			if(vec_length(dir) > projectilePreset->speed*time_step)
			{
				vec_normalize(dir, projectilePreset->speed*time_step);
			}
			else
			{
				projectileHitTrue = 1;
				projectile->lifetime = 0;
			}
			vec_add( projectile->pos2d, dir);
			mapGetVector3DFromVector2D(map, projectile->pos3d, projectile->pos2d);
			projectile->pos3d.z += (maploader_get_height(projectile->pos3d)+48-projectile->pos3d.z)*time_step;
			switch(projectile->presetID)
			{
				case UNIT_LERCHE:
				draw_point3d(projectile->pos3d, vector(255,0,255),100,32);	
				effect(p_lerche_proj_trail, 1, projectile->pos3d, dir);
				break;

				case UNIT_BABE:
				draw_point3d(projectile->pos3d, vector(0,190,225+random(30)),100,8);	
				break;

				case UNIT_SKULL:
				draw_point3d(projectile->pos3d, vector(random(50),255,255),100,44+random(10));	
				break;
				
				default:
				draw_point3d(projectile->pos3d, COLOR_GREEN,100,16);
			}
			
			projectile->lifetime -= time_step;
			if(projectile->lifetime <= 0)
			{
				if(projectileHitTrue)
				{
					draw_point3d(projectile->pos3d, COLOR_WHITE,100,64);
					if(projectile->presetID == UNIT_SKULL) effect(p_skull_proj_impact, 35, projectile->pos3d, nullvector);
					if(victim)
					{
						UNIT_PRESET* victimPreset = &unitPresets[victim->presetID];
						victim->HP -= projectilePreset->damage[victimPreset->armorType];
					}
				}
				currentProjectile--;
				map->numProjectiles[currentPlayer]--;
				if(map->numProjectiles[currentPlayer] > 0) memcpy(projectile, &(map->projectiles[currentPlayer][map->numProjectiles[currentPlayer]]), sizeof(PROJECTILE));
			}
		}
	}
}

void jpsGameUpdate(MAP* map)
{
	mapUpdateUnits(map);
	mapMoveUnits(map);
	mapUpdateProjectiles(map);
	draw_text2( str_printf(NULL, "Units: %d vs %d", map->numUnits[0], map->numUnits[1]), screen_size.x*0.5-50, 20, COLOR_WHITE);
}

void unitInitializeFromPreset(UNIT *unit, int presetID)
{
	if(presetID < 0 || presetID >= UNIT_CLASSES) error("unitInitializeFromPreset: invalid presetID!");
	unit->presetID = presetID;
	UNIT_PRESET* unitPreset = &unitPresets[unit->presetID];
	unit->HP = unitPreset->maxHP;
}

UNIT* jpsUnitCreate(int playerID, int unitType, ENTITY* ent)
{
	static int _uniqueID = 0;
	MAP* map = mapGetCurrent();
	if(playerID < 0 || playerID >= MAX_PLAYERS) error("jpsUnitCreate: bad playerID!");
	UNIT *unit = (UNIT*)sys_malloc(sizeof(UNIT));
	memset(unit, 0, sizeof(UNIT));
	unit->ent = ent;
	unit->isActive = 1;
	unit->playerID = playerID;
	unit->speedFac = 1;
	unit->allowLogic = 1;
	unit->targetReached = 1;
	unit->uniqueID = _uniqueID++;
	mapGetVector2DFromVector3D(map, unit->pos2d, ent->x);
	vec_set(unit->target2d, unit->pos2d);
	vec_set(unit->prevTarget2d, unit->pos2d);
	unitInitializeFromPreset(unit, unitType);
	unit->next = mapCurrent->unitFirst[playerID];
	map->unitFirst[playerID] = unit;
	map->numUnits[playerID]++;
	return unit;
}

void jpsUnitDestroy(UNIT* unit)
{
	if(!unit) return;
	if(unit->jpsPath) jpsPathDestroy(unit->jpsPath);
	MAP* map = mapGetCurrent();
	map->numUnits[unit->playerID]--;
	UNIT* prev = NULL;
	UNIT *otherUnit = map->unitFirst[unit->playerID];
	while(otherUnit)
	{
		UNIT* next = otherUnit->next;
		if(unit == otherUnit)
		{
			if(prev) prev->next = next;
			else map->unitFirst[unit->playerID] = next;
			sys_free(unit);
			return;
		}
		otherUnit = next;
	}
}

void unitSetTargetFromVector2D(MAP* map, UNIT* unit, VECTOR *vTarget)
{
	//cprintf3("\n unitSetTargetFromVector2D(%p, (%.1f,%.1f))", unit, (double)vTarget->x, (double)vTarget->y);
	if(!unit) return;
	unit->targetReached = 0;
	vec_set(unit->target2d, vTarget);
	if(vec_dist(unit->target2d, unit->prevTarget2d) > 0.2)
	{
		vec_set(unit->prevTarget2d, unit->target2d);
		unit->targetTile = mapTileGet(map, vTarget.x, vTarget.y);
		//cprintf1(" - unit->targetTile(%p)", unit->targetTile);
		//if(targetTile && unit->tile)
		if(!unit->jpsPath) unit->jpsPath = jpsPathCreate(4);
		mapJPSPathGet(map, unit->tile, unit->targetTile, unit->jpsPath);
	}
}


void unitAllSetTargetFromTile(MAP* map, int playerID, TILE* targetTile)
{
	UNIT *unit = map->unitFirst[playerID];
	while(unit)
	{
		unitSetTargetFromTile(map, unit, targetTile);
		unit = unit->next;
	}
}

void mapSaveToFile(MAP* map, char* filename)
{
	var fhandle = file_open_write(filename);
	if(!fhandle) return;
	int mapVersion = 1;
	file_var_write(fhandle, mapVersion);
	file_var_write(fhandle, map->size[0]);
	file_var_write(fhandle, map->size[1]);
	int i,j;
	for(i = 0; i < map->size[0]; i++)
	{
		for(j = 0; j < map->size[1]; j++)
		{
			TILE* tile = mapTileGet(map,i,j);
			file_var_write(fhandle, tile->value);
		}	
	}
	file_close(fhandle);
}

MATERIAL* jpsDummyNoFilter_mat =
{
	effect = "jpsDummyNoFilter.fx";
	flags = AUTORELOAD;
}

MAP* jpsMapLoadFromFile(char* filename)
{
	var fhandle = file_open_read(filename);
	if(!fhandle) return NULL;
	int mapVersion = file_var_read(fhandle);
	int sizeX = file_var_read(fhandle);
	int sizeY = file_var_read(fhandle);
	var tileSize = 64*4;
	MAP* map = mapCreate(sizeX, sizeY, vector(-sizeX/2*tileSize,-sizeY/2*tileSize,0), vector(sizeX/2*tileSize,sizeY/2*tileSize,0), tileSize);
	int i,j;
	for(i = 0; i < map->size[0]; i++)
	{
		for(j = 0; j < map->size[1]; j++)
		{
			TILE* tile = mapTileGet(map,i,j);
			tile->value = file_var_read(fhandle);
		}	
	}
	file_close(fhandle);
	mapJPSUpdate(map);
	mapUpdateBmap(map);

	if(0)
	{
		entJPSDummyPlane = ent_create("jpsPlane.mdl", vector(0,0,800), NULL);
		set(entJPSDummyPlane, PASSABLE | TRANSLUCENT);
		entJPSDummyPlane.flags2 |= UNTOUCHABLE;
		ent_setskin(entJPSDummyPlane, map->bmp, 1);
		vec_set(entJPSDummyPlane->scale_x, vector(sizeX/64.0*tileSize, sizeY/64.0*tileSize, 0));
		entJPSDummyPlane->material = jpsDummyNoFilter_mat;
	}

	return map;
}

BMAP* mapGetBitmap(MAP* map)
{
	if(!map) map = mapGetCurrent();
	if(!map) return NULL;
	return map->bmp;
}

void mapSetTileValueAtPos3D(MAP* map, VECTOR* pos3d, int value)
{
	TILE* tile = mapGetTileFromVector(map, pos3d);
	if(tile) tile->value = value;
}

int mapGetTileValueAtPos3D(MAP* map, VECTOR* pos3d)
{
	TILE* tile = mapGetTileFromVector(map, pos3d);
	if(tile) return tile->value;
	return -1;
}

TILE* mapGetEmptyTileForAI(MAP* map, int freeBorder, int jShift)
{
	int ishift = random(map->size[0]);
	//int jshift = random(map->size[0]);
	int jstart = 4+random(4);
	int i,j;
	for(j = jstart; j < map->size[1]; j++)
	{
		for(i = 0; i < map->size[0]; i++)
		{
			int i2 = (i+ishift)%map->size[0];
			//int j2 = (j+jshift)%map->size[1];
			int j2 = (j+jShift)%map->size[1];
			if(i2 >= 3 && i2 < map->size[0]-3 && j2 >= 3 && j2 < map->size[1]-3)
			{
				TILE* tile = mapTileGet(map,i2,j2);
				if(!tile->value)
				{
					int tileOkay = 8;
					if(freeBorder)
					{
						tileOkay = 0;
						int i3,j3;
						for(i3 = -1; i3 <= 1; i3++)
						{
							for(j3 = -1; j3 <= 1; j3++)
							{
								if(i3 || j3)
								{
									int i4 = i3+i2;
									int j4 = j3+j2;
									TILE* tile2 = mapTileGet(map,i4,j4);
									if(tile2)
									{
										if(!tile2->value) tileOkay++;
									}
								}
							}
						}
					}
					if(tileOkay == 8) return tile;
				}
			}
		}
	}
	return NULL;
}

///////////////////////////////
// jps.c
///////////////////////////////