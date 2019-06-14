#include "game.h"
#include "map_loader.h"
#include "ui_game.h"
#include <acknex.h>

void game_init(void)
{
	ui_game_init();
}

void game_open(void)
{
	maploader_load("the-core.png");

	ui_game_open();
}

void game_update(void)
{
	// render fake map
	{
		int x, y;
		for(x = 0; x < maploader_size_x(); x++)
		{
			for(y = 0; y < maploader_size_y(); y++)
			{
				COLOR c;
				vec_set(&c, vector(255,0,255));
				switch(maploader_tile_type(x, y))
				{
					case MAPLOADER_TILE_DEFAULT: vec_fill(&c, 255); break;
					case MAPLOADER_TILE_HOLE: vec_fill(&c, 32); break;
					case MAPLOADER_TILE_WATER: vec_set(&c, vector(255, 0, 0)); break;
					case MAPLOADER_TILE_LAVA: vec_fill(&c, vector(0, 0, 255)); break;
					case MAPLOADER_TILE_BARRIER: vec_fill(&c, vector(48, 48, 96)); break;
				}
				draw_quad(
					NULL,
					vector(8 * x, 8 * y, 0),
					NULL,
					vector(8, 8, 0),
					NULL,
					&c,
					100,
					0
				);
			}
		}
	}
	ui_game_update();
}

void game_close(void)
{
	ui_game_close();
}

bool game_is_done(void)
{
	return key_esc;
}
