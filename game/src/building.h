#ifndef BUILDING_H
#define BUILDING_H

#define BUILDING_NONE -1
#define BUILDING_PRESS 0
#define BUILDING_FARM 1
#define BUILDING_TREE 2
#define BUILDING_TOWER 3
#define BUILDING_BANK 4

#define BUILDING_NUMBER 5

char* buildingPlacement_assets[BUILDING_NUMBER];
char* buildingPlacement_constructionAssets[BUILDING_NUMBER];


void buildingPlacement_init();
void buildingPlacement_open();
void buildingPlacement_update();

void buildingPlacement_endConstruction();
void buildingPlacement_beginConstruction(int selection);
int buildingState();

#endif
