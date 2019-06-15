//////////////////////////////
// ai.c
//////////////////////////////

#include "ai.h"

// difficulty from 0 to 2, or more
void ai_reset(int difficulty)
{
	aiSystemInstance->difficulty = difficulty;
	aiSystemInstance->money = 10000;
	aiSystemInstance->escalationCounter = 0;
	aiSystemInstance->elapsedTime = 0;
	aiSystemInstance->stepTime = 0;
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
	MAP* map = mapGetCurrent();
	TILE* tile = mapGetEmptyTileForAI(map, 1);
	cprintf3("\n ai_spawn_building(%d) at frame %d: tile(%p)", buildingType, (int)total_frames, tile);
	if(tile)
	{
		spawner_spawn(0, mapGetVectorFromTile(map, NULL, tile), UNIT_ENEMY);
		aiSystemInstance->buildingCount[buildingType]++;
	}
}

void ai_update()
{
	aiSystemInstance->elapsedTime += 3*(1.0+aiSystemInstance->difficulty*0.5)*(float)time_step/16.0; // better than using time_frame in this case (game might slow down, AI should not play comparatively faster than)
	if(aiSystemInstance->elapsedTime > aiSystemInstance->stepTime)
	{
		aiSystemInstance->stepTime += 10;
		ai_spawn_building(0);
	}
}



//////////////////////////////
// ai.c
//////////////////////////////