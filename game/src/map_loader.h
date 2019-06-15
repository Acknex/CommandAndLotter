#ifndef MAP_LOADER_H
#define MAP_LOADER_H

#include "global.h"

int maploader_cellsize = 3; // cell size in quads
var maploader_trisize = 32; // triangle size

#define MAPLOADER_TILE_DEFAULT 0
#define MAPLOADER_TILE_HOLE    1
#define MAPLOADER_TILE_WATER   2
#define MAPLOADER_TILE_LAVA    3
#define MAPLOADER_TILE_BARRIER 4
#define MAPLOADER_TILE_STREET  5

void maploader_init(void);

bool maploader_has_map(void);

void maploader_load(char const * fileName);

int maploader_size_x(void);
int maploader_size_y(void);

int   maploader_tile_type(int x, int y);
float maploader_tile_vegetation(int x, int y);
float maploader_tile_height(int x, int y);

int   maploader_get_type(VECTOR * v);
float maploader_get_vegetation(VECTOR * v);
float maploader_get_height(VECTOR * v);

void maploader_pos_to_vec(VECTOR * v, int x, int y);
void maploader_pos_for_vec(VECTOR * v, int * x, int * y);

//! returns temporary vector or NULL when no hit
VECTOR * maploader_trace(VECTOR * start, VECTOR * end);

#endif // MAP_LOADER_H
