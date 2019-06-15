#include "map_loader.h"
#include <d3d9.h>
#include <stdio.h>

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

BMAP * maploader_terrain_digital = "terrain_digital.png";
BMAP * maploader_terrain_analogue = "terrain_analogue.png";
BMAP * maploader_terrain_splatter = "terrain_splatter.png";
BMAP * maploader_terrain_street = "terrain_street.png";

MATERIAL * maploader_terrain_material =
{
    effect = "terrain.fx";
    flags = PASS_SOLID | AUTORELOAD;
}

MATERIAL * maploader_water_material =
{
    effect = "water.fx";
    flags = TRANSLUCENT | AUTORELOAD;
}


void maploader_pos_to_vec(VECTOR * v, int x, int y)
{
    int size_x = maploader_cellsize * maploader.w + maploader_cellsize - 1;
    int size_y = maploader_cellsize * maploader.h + maploader_cellsize - 1;

    v->x = x * (maploader_trisize * maploader_cellsize) - maploader_trisize * size_x / 2;
    v->y = y * (maploader_trisize * maploader_cellsize) - maploader_trisize * size_y / 2;
    v->z = maploader_tile_height(x, y);
}

void maploader_pos_for_vec(VECTOR *v, int *x, int *y)
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
    bmap_to_mipmap(maploader_terrain_digital);
    bmap_to_mipmap(maploader_terrain_analogue);
    bmap_to_mipmap(maploader_terrain_splatter);
    bmap_to_mipmap(maploader_terrain_street);
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
    bmap_to_mipmap(bmp);
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

            /*
            int type = maploader_grey_to_type(col.red);

            if(x < 3 || y < 3 || x >= maploader.w-3 || y >= maploader.h -3) {
                col.red = 0xC0;
                type = MAPLOADER_TILE_HOLE;
                pixel = pixel_for_vec(&col, alpha, format);
                pixel_to_bmap(bmp, x, y, pixel);
            }
            */

			((maploader.cells)[maploader.w * y + x]).celltype = 0;
			((maploader.cells)[maploader.w * y + x]).vegetation = col.green / 255.0;
            ((maploader.cells)[maploader.w * y + x]).elevation = col.blue * 3;
		}
	}

	bmap_unlock(bmp);
	// bmap_remove(bmp);

	int size_x = maploader_cellsize * maploader.w + maploader_cellsize - 1;
	int size_y = maploader_cellsize * maploader.h + maploader_cellsize - 1;

    collision_mode = 0;

    you = ent_createterrain(
        bmp,
        vector(0, 0, 0),
        2,
        2,
        size_x * maploader_trisize
    );
    you.material = maploader_water_material;

    maploader.terrain = ent_createterrain(
        bmp,
        vector(0, 0, 0),
        size_x,
        size_y,
        maploader_trisize
    );

    int meshcnt = ent_status(maploader.terrain, 16);

    int iMesh;
    for(iMesh = 0; iMesh < meshcnt; iMesh++)
    {
        LPD3DXMESH mesh = ent_getmesh(maploader.terrain, iMesh, 0);

        D3DVERTEX * vertices;
        mesh->LockVertexBuffer(0, &vertices);

        int i;
        int cnt = mesh->GetNumVertices();
        for(i = 0; i < cnt; i++)
        {
            // CONTACT c;
            // ent_getvertex(maploader.terrain, &c, i);

            D3DVERTEX * v = &vertices[i];

            maploader_pos_for_vec(vector(v->x, v->z, 0), &x, &y);

            if(x <= 0) x = 0;
            if(y <= 0) y = 0;
            if(x >= maploader.w) x = maploader.w - 1;
            if(y >= maploader.h) y = maploader.h - 1;

            v->y = maploader_tile_height(x, y);
            int neighborX = minv(x + 1, maploader.w-1);
            int neighborY = minv(y + 1, maploader.h-1);
            var neighborXValue = maploader_tile_height(neighborX, y);
            var neighborYValue = maploader_tile_height(x, neighborY);

            VECTOR normalVector;
            normalVector.x = (neighborX - v->y)/255;
            normalVector.y = 1.0;
            normalVector.z = (neighborY - v->y)/255;
            vec_normalize(normalVector, 1.0);

            v->u1 = 154.0 * (float)x / (float)maploader.w / 256.0;
            v->v1 = 190.0 * (float)y / (float)maploader.h / 256.0;

            v->nx = (float)normalVector.x;
            v->ny = (float)normalVector.y;
            v->nz = (float)normalVector.z;

            // ent_setvertex(maploader.terrain, &c, i);
        }
        mesh->UnlockVertexBuffer();
    }

    set(maploader.terrain, PASSABLE);

    maploader.terrain.material = maploader_terrain_material;

    random_seed(1337);
    for(x = 0; x < maploader.w; x++)
    {
        for(y = 0; y < maploader.h; y++)
        {
            int type = maploader_tile_type(x, y);
            if(type != MAPLOADER_TILE_DEFAULT)
                continue;
            if(random(100) < 50)
                continue;
            float v = maploader_tile_vegetation(x, y);
            if(v < 0.2)
                continue;
            if(random(100) < pow(v, 6.0) * 100) {
                char const * boom = "tree01.mdl";
                var r = random(1);
                if(r > 0.66)
                    boom = "tree02.mdl";
                else if(r > 0.33)
                    boom = "tree03.mdl";

                you = ent_create(boom, nullvector, NULL);
                maploader_pos_to_vec(you->x, x, y);
                you->pan = random(360);
                you->tilt = random(30) - 15;
                // you->emask &= ~DYNAMIC;
            }
        }
    }

    maploader.terrain.clipfactor = 2;
    collision_mode = 1;

    if(key_c)
        c_updatehull(maploader.terrain, 0);
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
	maploader_pos_for_vec(v, &x, &y);
	return maploader_tile_type(x, y);
}

float maploader_get_vegetation(VECTOR * v)
{
	int x, y;
	maploader_pos_for_vec(v, &x, &y);
	return maploader_tile_vegetation(x, y);
}

float maploader_get_height(VECTOR * v)
{
	int x, y;
	maploader_pos_for_vec(v, &x, &y);
	return maploader_tile_height(x, y);
}

var maploader_dist2d(VECTOR * _a, VECTOR * _b)
{
    VECTOR a,b;
    vec_set(a, _a);
    vec_set(b, _b);
    a.z = 0;
    b.z = 0;
    return vec_dist(a, b);
}

var maploader_lerp(var a, var b, float f)
{
    return (1.0 - f) * a + f * b;
}

VECTOR * maploader_trace(VECTOR *_start, VECTOR *_end)
{
    VECTOR start, end;
    vec_set(start, _start);
    vec_set(end, _end);

    VECTOR iter;
    vec_set(iter, start);

    var total = maploader_dist2d(start, end);

    while(maploader_dist2d(iter, end) > maploader_trisize)
    {
        var a = maploader_dist2d(iter, end) / total;
        iter.z = maploader_lerp(end.z, start.z, a);
        if(iter.z < maploader_get_height(iter))
            return vector(iter.x, iter.y+1, iter.z);

        VECTOR dir;
        vec_diff(dir, end, start);
        dir.z = 0;
        vec_normalize(dir, maploader_trisize);

        vec_add(iter, dir);
    }
    if(iter.z < maploader_get_height(iter))
        return vector(iter.x, iter.y+1, iter.z);

    return NULL;
}
