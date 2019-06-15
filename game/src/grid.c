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
		
//		ent_create("cube.mdl", grid_getCellCenter_byID(i), NULL);
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

void grid_errorcheck(int id)
{
	if(id < 0 || id > grid_xSize*grid_ySize)
		error("ID out of bounds beim Zugriff auf die Grid");
}

/******************/

void grid_setState(int x, int y, int state)
{
	grid_setState_byID(grid_CoordToID(x,y), state);
}

void grid_resetState(int x, int y, int state)
{
	grid_resetState_byID(grid_CoordToID(x,y), state);
}

void grid_getState(int x, int y)
{
	return grid_getState_byID(grid_CoordToID(x,y));
}


void grid_setState_byID(int id, state)
{
	grid_errorcheck(id);
	grid_state[id] |= state;
	
}

void grid_resetState_byID(int id, state)
{
	grid_errorcheck(id);
	grid_state[id] &= ~state;
}

int grid_getState_byID(int id)
{
	grid_errorcheck(id);
	return grid_state[id];
}

/******************/

int grid_getCellID_byPos(VECTOR pos)
{
	int x = (pos.x + GRID_CELLSIZE*grid_xSize/2) / GRID_CELLSIZE;
	int y = (pos.y + GRID_CELLSIZE*grid_ySize/2) / GRID_CELLSIZE;
	
	return grid_CoordToID(x,y);
}

VECTOR *grid_getCellCenter(int x, int y) 
{
	grid_errorcheck(grid_CoordToID(x,y));
	x -= grid_xSize/2;
	y -= grid_ySize/2;
	return vector((x+0.5)*GRID_CELLSIZE, (y+0.5)*GRID_CELLSIZE, 400);
}

VECTOR *grid_getCellCenter_byID(int ID) 
{
	return grid_getCellCenter(grid_idx2X(ID), grid_idx2Y(ID));
}
