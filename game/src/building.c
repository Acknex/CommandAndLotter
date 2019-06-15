#include "building.h"

#include "global.h"
#include "framework.h"
#include "spawner.h"
#include "jps.h"

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
		TILE* tile = mapGetTileFromVector(mapCurrent, _hit);
		if(tile != NULL)
		{
			VECTOR targetPos;
			mapGetVectorFromTile(mapCurrent, &targetPos, tile);
			targetPos.z = _hit.z;
			vec_set(&buildingPlacement_previewModel->x, targetPos);
		}	
	}
}

	

void buildingPlacement_beginConstruction(int selection)
{
	buildingPlacement_selection = selection;
	buildingPlacement_previewModel = ent_create(buildingPlacement_constructionAssets[selection], nullvector, NULL);
	buildingPlacement_previewModel->flags |= TRANSLUCENT;
	buildingPlacement_previewModel->alpha = 70;
}

void buildingPlacement_endConstruction()
{
	ptr_remove(buildingPlacement_previewModel);
	buildingPlacement_previewModel = NULL;
	buildingPlacement_selection = BUILDING_NONE;
}

void buildingPlacement_placeConstruction()
{
	switch(buildingPlacement_selection)
	{
		case BUILDING_TOWER:
		//case ..
			spawner_spawn(buildingPlacement_selection, &buildingPlacement_previewModel->x, SPAWNER_PLAYER);
		//case nichtspawnergebaeude
		break;
		default:
			error("unbekannter Gebauedetyp");
	}
	
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