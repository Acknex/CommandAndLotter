#include "grid.h"

int *grid_state;
int grid_xSize;
int grid_ySize;

void grid_init()
{
	grid_state = NULL;
}

void grid_open(int xSize, int ySize)
{
	grid_state = sys_malloc(sizeof(int)*xSize*ySize);
	grid_xSize = xSize;
	grid_ySize = ySize;
	
	int i;
	int on = 0;
	for(i = 0; i<xSize*ySize; ++i)
	{
		on = 1-on;
		grid_state[i] = on;
	}
}

void grid_close()
{
	sys_free(grid_state);
	grid_state = NULL;
}

/******************/

int grid_CoordToID(int x, int y)
{
	return x + y*grid_xSize;
}

int grid_idx2X(int idx)
{
	return idx % grid_xSize;
}
int grid_idx2Y(int idx)
{
	return (int)(idx / grid_ySize);
}

/******************/

void grid_setState(int x, int y, int state)
{
	grid_setState_byID(grid_CoordToID(x,y), state);
}

void grid_getState(int x, int y)
{
	return grid_getState_byID(grid_CoordToID(x,y));
}


void grid_setState_byID(int id, state)
{
	grid_state[id] = state;
	
}
int grid_getState_byID(int id)
{
	return grid_state[id];
	
}

/******************/

