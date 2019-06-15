//////////////////////////////
// ai.h
//////////////////////////////

#ifndef AI_H
	#define AI_H
	
	#define AI_MAX_BUILDINGS_PER_TYPE 8
	struct _AI_SYSTEM
	{
		int difficulty;
		int money;
		int escalationCounter;
		float elapsedTime, stepTime, unitBuildTimer;
		int buildingCount[BUILDING_NUMBER];
		ENTITY* entBuildings[BUILDING_NUMBER][AI_MAX_BUILDINGS_PER_TYPE];
	};
	typedef struct _AI_SYSTEM AI_SYSTEM;
	AI_SYSTEM *aiSystemInstance = NULL;

	// difficulty from 0 to 2, or more
	void ai_reset(int difficulty);

	void ai_init(int difficulty);

	void ai_spawn_building(int buildingType);

	void ai_update();

#endif

//////////////////////////////
// ai.h
//////////////////////////////