//////////////////////////////
// ai.c
//////////////////////////////

#include "ai.h"

// difficulty from 0 to 2, or more
void ai_reset(int difficulty)
{
	memset(aiSystemInstance, 0, sizeof(AI_SYSTEM));
	aiSystemInstance->difficulty = difficulty;
	aiSystemInstance->money = 10000;
	aiSystemInstance->escalationCounter = 0;
	aiSystemInstance->elapsedTime = 0;
	aiSystemInstance->stepTime = 0;
	aiSystemInstance->unitBuildTimer = -10;
	int i;
	for(i = 0; i < BUILDING_NUMBER; i++) aiSystemInstance->buildingCount[i] = 0;
}

void ai_init(int difficulty)
{
	aiSystemInstance = (AI_SYSTEM*)sys_malloc(sizeof(AI_SYSTEM));
	memset(aiSystemInstance, 0, sizeof(AI_SYSTEM));
	ai_reset(difficulty);
}

void ai_spawn_building(int buildingType)
{
	if(aiSystemInstance->buildingCount[buildingType] >= AI_MAX_BUILDINGS_PER_TYPE) return;
	MAP* map = mapGetCurrent();
	TILE* tile = mapGetEmptyTileForAI(map, 1);
	cprintf3("\n ai_spawn_building(%d) at frame %d: tile(%p)", buildingType, (int)total_frames, tile);
	if(tile)
	{
		aiSystemInstance->entBuildings[buildingType][aiSystemInstance->buildingCount[buildingType]++] = spawner_spawn(0, mapGetVectorFromTile(map, NULL, tile), UNIT_ENEMY);
	}
}

void ai_spawn_unit(int unitType)
{
	int buildingType = 0; // buildingType == unitType?
	if(aiSystemInstance->buildingCount[buildingType] <= 0) return;
	ENTITY* ptr = aiSystemInstance->entBuildings[buildingType][(int)random(aiSystemInstance->buildingCount[buildingType])];
	ptr->SPAWNER_SPAWNANGLE += 137.5;
	ptr->SPAWNER_SPAWNANGLE %= 360;
	VECTOR* targetPos = vector(700,0,0);
	VECTOR* angle = vector(ang(ptr->SPAWNER_SPAWNANGLE), 0, 0);
	vec_rotate(targetPos, angle);
	//vec_add(targetPos, ptr->x);
	
	unit_spawn(unitType, ptr->x, targetPos, 1);
}

void ai_update()
{
	float timeAdd = 2*(1.0+aiSystemInstance->difficulty*0.5)*(float)time_step/16.0; // better than using time_frame in this case (game might slow down, AI should not play comparatively faster than)
	aiSystemInstance->elapsedTime += timeAdd;
	if(aiSystemInstance->elapsedTime > aiSystemInstance->stepTime)
	{
		aiSystemInstance->stepTime += 10;
		ai_spawn_building(0);
	}
	aiSystemInstance->unitBuildTimer += timeAdd;
	if(aiSystemInstance->unitBuildTimer > 4)
	{
		aiSystemInstance->unitBuildTimer -= 4;
		ai_spawn_unit(0);
	}
}



//////////////////////////////
// ai.c
//////////////////////////////