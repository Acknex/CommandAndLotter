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

MATERIAL * maploader_material =
{
  flags = AUTORELOAD;
  emissive_blue = 255;
}

void maploader_get_tile_pos(VECTOR * v, int * x, int * y)
{
	int size_x = maploader_cellsize * maploader.w + maploader_cellsize - 1;
	int size_y = maploader_cellsize * maploader.h + maploader_cellsize - 1;

	*x = (v->x + maploader_trisize * size_x / 2) / (maploader_trisize * maploader_cellsize);
	*y = (v->y + maploader_trisize * size_y / 2) / (maploader_trisize * maploader_cellsize);

	if(*x <= 0) *x = 0;
	if(*y <= 0) *y = 0;
	if(*x >= maploader.w) *x = maploader.w - 1;
	if(*y >= maploader.h) *y = maploader.h - 1;
}

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

	int size_x = maploader_cellsize * maploader.w + maploader_cellsize - 1;
	int size_y = maploader_cellsize * maploader.h + maploader_cellsize - 1;

	maploader.terrain = ent_createterrain(
		bmp,
		vector(0, 0, 0),
		size_x,
		size_y,
		maploader_trisize
	);

	int i;
	int cnt = ent_status(maploader.terrain, 1);
	for(i = 0; i < cnt; i++)
	{
		CONTACT c;
		ent_getvertex(maploader.terrain, &c, i);

		D3DVERTEX * v = c.v;

		maploader_get_tile_pos(vector(v->x, v->z, 0), &x, &y);

//		diag("x=");
//		diag(str_for_int(NULL, x));
//		diag(" y=");
//		diag(str_for_int(NULL, y));
//		diag("\n");

		if(x <= 0) x = 0;
		if(y <= 0) y = 0;
		if(x >= maploader.w) x = maploader.w - 1;
		if(y >= maploader.h) y = maploader.h - 1;

		v->y = maploader_tile_height(x, y);

		v->u1 = (float)x / (float)maploader.w;
		v->v1 = (float)y / (float)maploader.h;

		ent_setvertex(maploader.terrain, &c, i);
	}

	c_updatehull(maploader.terrain, 0);

	maploader.terrain = maploader_material;

	effect_load(maploader_material, "terrain.fx");
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

int   maploader_get_type(VECTOR * v)
{
	int x, y;
	maploader_get_tile_pos(v, &x, &y);
	return maploader_tile_type(x, y);
}

float maploader_get_vegetation(VECTOR * v)
{
	int x, y;
	maploader_get_tile_pos(v, &x, &y);
	return maploader_tile_vegetation(x, y);
}

float maploader_get_height(VECTOR * v)
{
	int x, y;
	maploader_get_tile_pos(v, &x, &y);
	return maploader_tile_height(x, y);
}
