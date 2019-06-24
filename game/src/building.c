#include "building.h"

#include "global.h"
#include "framework.h"
#include "spawner.h"
#include "jps.h"

ENTITY *buildingPlacement_previewModel;
int buildingPlacement_selection;

int buildingState()
{
	return buildingPlacement_selection;
}


void buildingPlacement_init()
{
	buildingPlacement_assets[BUILDING_PRESS] = "sputnik_trash_press.mdl";
	buildingPlacement_constructionAssets[BUILDING_PRESS] = "sputnik_trash_press_wf.mdl";

	buildingPlacement_assets[BUILDING_FARM] = "lark_farm.mdl";
	buildingPlacement_constructionAssets[BUILDING_FARM] = "lark_farm_wireframe.mdl";

	buildingPlacement_assets[BUILDING_TREE] = "eye_tree_you.mdl";
	buildingPlacement_constructionAssets[BUILDING_TREE] = "eye_tree_you_wireframe.mdl";

	buildingPlacement_assets[BUILDING_TOWER] = "the_tower.mdl";
	buildingPlacement_constructionAssets[BUILDING_TOWER] = "the_tower_wireframe.mdl";

	buildingPlacement_assets[BUILDING_BANK] = "bank_of_zorro.mdl";
	buildingPlacement_constructionAssets[BUILDING_BANK] = "bank_of_zorro_wireframe.mdl";
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
			if(!tile->value)
			{
				VECTOR targetPos;
				mapGetVectorFromTile(mapCurrent, &targetPos, tile);
				targetPos.z = _hit.z;
				vec_set(&buildingPlacement_previewModel->x, targetPos);
			}
		}
	}
}



void buildingPlacement_beginConstruction(int selection)
{
	if(  buildingPlacement_selection != BUILDING_NONE )
	{
		buildingPlacement_endConstruction();
	}
	
	buildingPlacement_selection = selection;
	buildingPlacement_previewModel = ent_create(buildingPlacement_constructionAssets[selection], nullvector, NULL);
	buildingPlacement_previewModel->flags |= TRANSLUCENT;
	buildingPlacement_previewModel->alpha = 70;
	buildingPlacement_previewModel->pan = random(360);
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
		case BUILDING_PRESS:
		case BUILDING_FARM:
		case BUILDING_TREE:
		case BUILDING_TOWER:
		case BUILDING_BANK:
		//case ..
		if(!buildingPlacement_previewModel) error("buildingPlacement_placeConstruction: !buildingPlacement_previewModel !!!");
		spawner_spawn(buildingPlacement_selection, &buildingPlacement_previewModel->x, buildingPlacement_previewModel->pan, PLAYER_ID_PLAYER);
		//case nichtspawnergebaeude
		break;
		default:
		error("unbekannter Gebauedetyp");
	}

	buildingPlacement_endConstruction();
}

void buildingPlacement_update()
{
	if(buildingPlacement_selection == BUILDING_NONE && !mouse_left)
	{
		if(key_1)
		buildingPlacement_beginConstruction(BUILDING_PRESS);
		if(key_2)
		buildingPlacement_beginConstruction(BUILDING_FARM);
		if(key_3)
		buildingPlacement_beginConstruction(BUILDING_TREE);
		if(key_4)
		buildingPlacement_beginConstruction(BUILDING_TOWER);
		if(key_5)
		buildingPlacement_beginConstruction(BUILDING_BANK);
	}

	if(buildingPlacement_selection != BUILDING_NONE)
	{
		buildingPlacement_movePreview();
		if(mouse_left && !mouse_panel)
		buildingPlacement_placeConstruction();
		if(mouse_right)
		buildingPlacement_endConstruction();
	}
}
