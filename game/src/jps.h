#ifndef JPS_H
	#define JPS_H
	///////////////////////////////
	// jps.h
	///////////////////////////////
	/*
	- create a MAP and fill it appropriately: (value != 0) -> tile is non-traversable
	- call mapJPSUpdate() after that stage or when some tiles change in traversability
	- call mapJPSPathGet() with start and target tiles
	- either provide a pointer to a JPSPATH or use or copy the map's JPSPATH 
	*/
	///////////////////////////////
	// configurable stuff

	//#define JPS_DEBUG
	int jpsExtraWidth = 0; // either 0 or 1 (for now). at 1 JPS nodes are placed at one tile extra distance to corners, resulting in up to 3 times as many nodes though

	///////////////////////////////

	struct _LIST
	{
		void* data;
		struct _LIST *next;
	};
	typedef struct _LIST LIST;

	#define MAX_UNITS_PER_TILE 16
	#define MAX_COST 9999 // keep this somewhat low in case var calculations need to be/ are done with it
	#define TILE_FLAG_JPS (1<<0)
	#define TILE_FLAG_CURRENT_PATH (1<<1)
	struct _TILE
	{
		int pos[2];
		int value;
		int value2;
		int flags;
		//int jpsDist[8];
		int jpsType;
		int numNeighbors;
		LIST *neighborList;
		int *neighborCost;
		int targetCost;
		int targetReachable;
		int currentCost;
		int totalCost;
		LIST *unitList;
		int numUnits;
		void *unitArray[MAX_UNITS_PER_TILE];
		struct _TILE *prev;
	};
	typedef struct _TILE TILE;
	TILE* startTile = NULL;
	TILE* targetTile = NULL;

	struct _JPSPATH
	{
		int length, maxLength, currentNode; // maxLength == size of tiles pointer array, for reusing PATHs
		TILE** tiles;
		TILE* startTile;
		TILE* targetTile;
	};
	typedef struct _JPSPATH JPSPATH; // PATH might be an acknex keyword

	struct _MAP
	{
		int size[2];
		TILE* tiles;
		BMAP* bmp;
		LIST* JPSTileList;
		JPSPATH *jpsPath;
		// quant and 3d stuff
		VECTOR vMin, vMax, vSize;
		var tileSize;
	};
	typedef struct _MAP MAP;
	MAP* mapCurrent = NULL;

	#define MAX_PRESETS 2
	struct _UNIT_PRESET
	{
		int ID;
		char file[32];
		int maxSpeed;
		int maxHP;
		var radius;
	};
	typedef struct _UNIT_PRESET UNIT_PRESET;
	UNIT_PRESET unitPresets[MAX_PRESETS];

	#define MAX_TURNS 2
	struct _UNIT
	{
		int presetID;
		int playerID;
		int isActive;
		int HP;
		int speed;
		VECTOR pos2d, pos3d;
		VECTOR target2d, prevTarget2d, currentTarget2d;
		ENTITY* ent;
		TILE* tile;
		TILE* prevTile;
		TILE* targetTile;
		JPSPATH *jpsPath;
		struct _UNIT *next;
	};
	typedef struct _UNIT UNIT;

	struct _PROJECTILE_PRESET
	{
		int ID;
		int damage;
		int range;
		// particles?
	};
	typedef struct _PROJECTILE_PRESET PROJECTILE_PRESET;

	struct _PROJECTILE
	{
		int presetID;
		int playerID;
		int isActive;
		int lifetime;
		VECTOR pos2d, pos3d; //[MAX_TURNS];
		struct _PROJECTILE *next;
	};
	typedef struct _PROJECTILE PROJECTILE;

	#define MAX_PLAYERS 2
	struct _GAME_STATE
	{
		// buildings, similar to units
		UNIT *unitFirst[MAX_PLAYERS];
		PROJECTILE *projectileFirst[MAX_PLAYERS];
	};
	typedef struct _GAME_STATE GAME_STATE;
	GAME_STATE gameState;

	/////////////////////////////////////////////////

	#include "console.h"

	/////////////////////////////////////////////////

	#define ITF (int)total_frames
	double dJPSTime = 0;
	int offset2D[16] = { -1,0, 1,0, 0,-1, 0,1,   -1,-1, -1,1, 1,1, 1,-1 }; // cross first, then diagonals

	/////////////////////////////////////////////////
	// available functions

	JPSPATH *jpsPathCreate(int maxLength);

	MAP *mapCreate(int sizeX, int sizeY);

	// randomly generate a dummy map
	void mapFillTest(MAP* map);

	TILE* mapTileGet(MAP* map, int x, int y);

	int mapTraceDo(MAP* map, int *posFrom, int *posTo, int mode);

	// call on map updates
	void mapJPSUpdate(MAP* map);

	void mapUpdateBmap(MAP* map);

	// primary JPS function
	int mapJPSPathGet(MAP* map, TILE* startTile, TILE *targetTile, JPSPATH *jpsPathOut);

	/////////////////////////////////////////////////
	// functions used iternally

	LIST* listAdd(LIST* current, void* data);

	LIST* listAddTileSorted(LIST* current, TILE* tile, int sortMethod);

	void listDestroy(LIST* list, int mode);

	void draw_line2(VECTOR* vFrom, VECTOR* vTo, COLOR* color, var alpha);

	void mapTileFreeNeighborData(MAP* map, TILE* sourceTile);

	var mapTileCostCalculate(MAP* map, TILE* tile1, TILE* tile2);

	void mapTileCalculateVisibilityForTile(MAP* map, TILE* sourceTile);

	void tileResetJPSData(TILE* tile);

	///////////////////////////////

	void presetsInit();

	VECTOR* mapGetVectorFromTile(MAP* map, VECTOR* v, TILE* tile);

	VECTOR* mapGetVector2DFromVector3D(MAP* map, VECTOR* v, VECTOR* v2d);

	TILE* mapGetTileFromVector(MAP* map, VECTOR* v);

	void mapUpdateUnits(MAP* map);

	#define POINTER_ARRAY_MAX 1024
	void* pointerArray[POINTER_ARRAY_MAX]; // yes!
	int pointerArrayNum = 0;

	LIST* mapGetNearbyUnits_listbased(MAP* map, TILE* sourceTile, int range);

	int mapGetNearbyUnits(MAP* map, TILE* sourceTile, int range);

	VECTOR* unitFlockingSpeedGet(MAP* map, UNIT* unit, VECTOR* v);

	int sphereBoxCollision(VECTOR* spherePos2D, VECTOR* vSpeed, float sphereRadius, float boxMinX, float boxMinY, float boxMaxX, float boxMaxY);

	int unitMoveWithCollision(MAP* map, UNIT* unit, VECTOR *vSpeed);

	void unitMove(MAP* map, UNIT* unit);

	void mapMoveUnits(MAP* map);

	void gameUpdate(MAP* map);

	void unitInitializeFromPreset(UNIT *unit, int presetID);

	UNIT* unitCreate(int playerID, int presetID, VECTOR* pos2d);

	void unitSetTargetFromTile(MAP* map, UNIT* unit, TILE* targetTile);

	void unitAllSetTargetFromTile(MAP* map, int playerID, TILE* targetTile);

	void mapSaveToFile(MAP* map, char* filename);

	MAP* mapLoadFromFile(char* filename);

	///////////////////////////////
	// jps.h
	///////////////////////////////
#endif