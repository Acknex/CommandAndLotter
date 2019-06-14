#include "map_loader.h"

typedef struct
{
	int celltype;
	float vegetation;
	float elevation;
} maploader_cell;

struct maploader_t
{
	int w, h;
	maploader_cell * cells;
};

struct maploader_t maploader;

void maploader_init()
{
	maploader.cells = NULL;
}

int maploader_grey_to_type(int gray)
{
	if(gray < 0x20)
		return MAPLOADER_TILE_BARRIER;
	else if(gray <= 0x60)
		return MAPLOADER_TILE_WATER;
	else if(gray <= 0xA0)
		return MAPLOADER_TILE_LAVA;
	else if(gray <= 0xE0)
		return MAPLOADER_TILE_HOLE;
	else
		return MAPLOADER_TILE_DEFAULT;
}

void maploader_load(char const * fileName)
{
	if(maploader.cells) {
		sys_free(maploader.cells);
	}
	BMAP * bmp = bmap_create(fileName);
	if(bmp == NULL)
		error("failed to load level");
	var format = bmap_lock(bmp, 0);

	maploader.w = bmap_width(bmp);
	maploader.h = bmap_height(bmp);
	maploader.cells = sys_malloc(sizeof(maploader_cell) * maploader.w * maploader.h);

	int x, y;
	for(x = 0; x < maploader.w; x++)
	{
		for(y = 0; y < maploader.h; y++)
		{
			var pixel = pixel_for_bmap(bmp, x, y);

			COLOR col;
			var alpha;
			pixel_to_vec(&col, &alpha, format, pixel);

			((maploader.cells)[maploader.w * y + x]).celltype = maploader_grey_to_type(col.red);
			((maploader.cells)[maploader.w * y + x]).vegetation = col.green / 255.0;
			((maploader.cells)[maploader.w * y + x]).elevation = col.blue;
		}
	}

	bmap_unlock(bmp);
	bmap_remove(bmp);
}

bool maploader_has_map()
{
	return (maploader.cells != NULL);
}

int maploader_size_x()
{
	return maploader.w;
}

int maploader_size_y()
{
	return maploader.h;
}

int maploader_tile_type(int x, int y)
{
	return ((maploader.cells)[maploader.w * y + x]).celltype;
}

float maploader_tile_vegetation(int x, int y)
{
	return ((maploader.cells)[maploader.w * y + x]).vegetation;
}

float maploader_tile_height(int x, int y)
{
	return ((maploader.cells)[maploader.w * y + x]).elevation;
}
