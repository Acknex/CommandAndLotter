#include "effects2d.h"


struct effects2d_animation
{
    BMAP * frames[10];
    int frame_count;
    var speed;
};

struct effects2d_particle
{
    int x, y;
    int offset;
};

struct effects2d_t
{
    struct effects2d_animation animations[3];
};

effects2d_t effects2d;

void effects2d_init()
{

}

void effects2d_open()
{

}

void effects2d_update()
{

}

void effects2d_close()
{

}

void effects2d_spawn(int x, int y, int type)
{

}
