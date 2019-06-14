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
	ENTITY * terrain;
};

struct maploader_t maploader;

void maploader_init()
{
	maploader.cells = NULL;
}

int maploader_grey_to_type(int gray)
{
	if(gray < 0x10)
		return MAPLOADER_TILE_BARRIER;
	if(gray < 0x30)
		return MAPLOADER_TILE_STREET;
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
	// bmap_remove(bmp);

	int cellsize = 3;
	var trisize = 32;

	int size_x = cellsize * maploader.w + cellsize - 1;
	int size_y = cellsize * maploader.h + cellsize - 1;

	maploader.terrain = ent_createterrain(
		bmp,
		vector(0, 0, 0),
		size_x,
		size_y,
		trisize
	);

	int i;
	int cnt = ent_status(maploader.terrain, 1);
	for(i = 1; i <= cnt; i++)
	{
		CONTACT c;
		ent_getvertex(maploader.terrain, &c, i);

		x = (c.x + maploader.terrain->x + trisize * size_x / 2) / (trisize * cellsize);
		y = (c.y + maploader.terrain->y + trisize * size_y / 2) / (trisize * cellsize);

//		diag("x=");
//		diag(str_for_int(NULL, x));
//		diag(" y=");
//		diag(str_for_int(NULL, y));
//		diag("\n");

		if(x <= 0) x = 0;
		if(y <= 0) y = 0;
		if(x >= maploader.w) x = maploader.w - 1;
		if(y >= maploader.h) y = maploader.h - 1;

		c.v->y = maploader_tile_height(x, y);

		c.v->u1 = (float)x / (float)maploader.w;
		c.v->v1 = (float)y / (float)maploader.h;

		ent_setvertex(maploader.terrain, &c, i);
	}
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
