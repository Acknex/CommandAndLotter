#ifndef BUILDING_H
#define BUILDING_H

#define BUILDING_NONE -1
#define BUILDING_TOWER 0

#define BUILDING_NUMBER 1

char* buildingPlacement_assets[BUILDING_NUMBER];
char* buildingPlacement_constructionAssets[BUILDING_NUMBER];


void buildingPlacement_init();
void buildingPlacement_open();
void buildingPlacement_update();

#endif