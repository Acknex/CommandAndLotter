#ifndef GRID_H
#define GRID_H

#define GRID_CELLSIZE 100

//bitmask
#define GRID_BLOCKED 1
#define GRID_SCOUTED 2

void grid_init();
void grid_open(int xSize, int ySize);
void grid_close();

int grid_CoordToID(int x, int y);

void grid_setState(int x, int y, int state);
void grid_resetState(int x, int y, int state);
int grid_getState(int x, int y);


void grid_setState_byID(int id, state);
void grid_resetState_byID(int id, state);
int grid_getState_byID(int id);

int grid_getCellID_byPos(VECTOR);
VECTOR grid_getCellPos_byID(int);

#endif