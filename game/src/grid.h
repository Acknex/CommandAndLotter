#ifndef GRID_H
#define GRID_H

#define GRID_CELLSIZE 100

//bitmask
#define GRID_CLEAR 0
#define GRID_BLOCKED 1

void grid_init();
void grid_open(int xSize, int ySize);
void grid_close();

void grid_setState(int x, int y, int state);
int grid_getState(int x, int y);

int grid_CoordToID(int x, int y);

void grid_setState_byID(int id, state);
int grid_getState_byID(int id);

int grid_getCellID_byPos(VECTOR);
VECTOR grid_getCellPos_byID(int);

#endif