#ifndef MAP_LOADER_H
#define MAP_LOADER_H

#include "global.h"

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

#endif // MAP_LOADER_H
