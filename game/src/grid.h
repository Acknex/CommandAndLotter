#ifndef GRID_H
#define GRID_H

//bitmask
#define GRID_CLEAR 0
#define GRID_BLOCKED 1

void grid_init();
void grid_open(int xSize, int ySize);
void grid_close();

void grid_setState(int x, int y, int state);
int grid_getState(int x, int y);

#endif