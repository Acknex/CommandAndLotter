#include "building.h"

char* buildingPlacement_assets[BUILDING_NUMBER];
char* buildingPlacement_constructionAssets[BUILDING_NUMBER];

ENTITY *buildingPlacement_previewModel;
int buildingPlacement_selection;


void buildingPlacement_init()
{
	buildingPlacement_assets[0] = "the_tower.mdl";
	
	buildingPlacement_constructionAssets[0] = "the_tower_wireframe.mdl";
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
	{
		DEBUG_VAR(_hit->x, 100);
		DEBUG_VAR(_hit->y, 130);
		DEBUG_VAR(_hit->z, 160);
		vec_set(&buildingPlacement_previewModel->x, _hit);	
	}
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
	ENTITY *constructionSite = ent_create(asset, buildingPlacement_previewModel->x, NULL);
	
	
	buildingPlacement_endConstruction();
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
}