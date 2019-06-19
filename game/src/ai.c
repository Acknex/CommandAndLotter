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
		aiSystemInstance->entBuildings[buildingType][aiSystemInstance->buildingCount[buildingType]++] = spawner_spawn(0, mapGetVectorFromTile(map, NULL, tile), random(360), PLAYER_ID_AI);
	}
}

void ai_spawn_unit(int unitType)
{
	int buildingType = 0; // buildingType == unitType? <-- yes.
	if(aiSystemInstance->buildingCount[buildingType] <= 0) return;
	ENTITY* ptr = aiSystemInstance->entBuildings[buildingType][(int)random(aiSystemInstance->buildingCount[buildingType])];

//todo: AI logic should check z amount before just trying to produce new units	
	spawner_produce(ptr);
}

void ai_update()
{
	static int dummyUnitCountForNow = 0;
	if(dummyUnitCountForNow >= 50) return;
	float timeAdd = (1.0+aiSystemInstance->difficulty*0.5)*(float)time_step/16.0; // better than using time_frame in this case (game might slow down, AI should not play comparatively faster than)
	aiSystemInstance->elapsedTime += timeAdd;
	if(aiSystemInstance->elapsedTime > aiSystemInstance->stepTime)
	{
		aiSystemInstance->stepTime += 10;
		ai_spawn_building(0);
	}
	aiSystemInstance->unitBuildTimer += timeAdd;
	if(aiSystemInstance->unitBuildTimer > 6)
	{
		aiSystemInstance->unitBuildTimer -= 6;
		ai_spawn_unit(0);
		dummyUnitCountForNow++;
	}
}



//////////////////////////////
// ai.c
//////////////////////////////