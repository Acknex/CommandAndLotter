#include "building.h"

#include "global.h"
#include "framework.h"
#include "spawner.h"
#include "enemy_hit.h"

#define BUILDING_TYPE skill[20]
#define BUILDING_WORK_LEFT skill[21]

char* buildingPlacement_assets[BUILDING_NUMBER];
char* buildingPlacement_constructionAssets[BUILDING_NUMBER];
var buildingPlacement_buildtimes[BUILDING_NUMBER];

ENTITY *buildingPlacement_previewModel;
int buildingPlacement_selection;


void buildingPlacement_init()
{
	buildingPlacement_assets[0] = "the_tower.mdl";
	buildingPlacement_constructionAssets[0] = "the_tower_wireframe.mdl";
	buildingPlacement_buildtimes[0] = 10;
}

void buildingPlacement_open()
{
	buildingPlacement_previewModel = NULL;
	buildingPlacement_selection = BUILDING_NONE;
}


void buildingPlacement_movePreview()
{
	VECTOR *_hit = get_pos_under_cursor();
	if(_hit != NULL)
		vec_set(&buildingPlacement_previewModel->x, _hit);	
}



void ConstructionSite()
{
   framework_setup(my, SUBSYSTEM_CONSTRUCTION);
//	ENEMY_HIT_init(my);
	set(my, SHADOW);
	c_setminmax(me);
}


void buildingPlacement_beginConstruction(int selection)
{
	buildingPlacement_selection = selection;
	buildingPlacement_previewModel = ent_create(buildingPlacement_constructionAssets[selection], nullvector, NULL);
	buildingPlacement_previewModel->flags |= TRANSLUCENT;
	buildingPlacement_previewModel->alpha = 50;
}

void buildingPlacement_endConstruction()
{
	ptr_remove(buildingPlacement_previewModel);
	buildingPlacement_previewModel = NULL;
	buildingPlacement_selection = BUILDING_NONE;
}

void buildingPlacement_placeConstruction()
{
	char* asset = buildingPlacement_constructionAssets[buildingPlacement_selection];
	ENTITY *constructionSite = ent_create(asset, buildingPlacement_previewModel->x, ConstructionSite);
	constructionSite->BUILDING_TYPE = buildingPlacement_selection;
	constructionSite->BUILDING_WORK_LEFT = buildingPlacement_buildtimes[buildingPlacement_selection];
	
	buildingPlacement_endConstruction();
}


void buildingPlacement_constructionFinished(ENTITY *construction)
{
	spawner_spawn(construction->BUILDING_TYPE, &construction->x, SPAWNER_PLAYER);
}

void buildingPlacement_update()
{
	if(buildingPlacement_selection == BUILDING_NONE) 
	{
		if(key_t)
			buildingPlacement_beginConstruction(BUILDING_TOWER);
	}
	
	if(buildingPlacement_selection != BUILDING_NONE) 
	{
		buildingPlacement_movePreview();
		if(mouse_left) 
			buildingPlacement_placeConstruction();
		if(mouse_right) 
			buildingPlacement_endConstruction();
	}
	
	
	ENTITY *ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_CONSTRUCTION)
	{
		ptr->BUILDING_WORK_LEFT -= time_step;
		if(ptr->BUILDING_WORK_LEFT <= 0)
		{
			buildingPlacement_constructionFinished(ptr);
			framework_remove(ptr);
		}
	}
}