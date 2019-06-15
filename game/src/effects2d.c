#include "effects2d.h"
#include <acknex.h>

#define EFFECTS2D_PARTICLE_COUNT 50

struct effects2d_animation
{
    BMAP * frames[10];
    int frame_count;
    var speed;
};

struct effects2d_particle
{
    int x, y;
    int animation;
    var frame;
};

struct effects2d_t
{
    struct effects2d_animation animations[3];

    struct effects2d_particle particles[50];
    int particle_count;
};

struct effects2d_t effects2d;

void effects2d_init()
{
    // GOTO
    effects2d.animations[0].frame_count = 5;
    effects2d.animations[0].speed = 1;
    effects2d.animations[0].frames[0] = bmap_create("Cursor1.png");
    effects2d.animations[0].frames[1] = bmap_create("Cursor2.png");
    effects2d.animations[0].frames[2] = bmap_create("Cursor3.png");
    effects2d.animations[0].frames[3] = bmap_create("Cursor4.png");
    effects2d.animations[0].frames[4] = bmap_create("Cursor5.png");

    effects2d.particle_count = 0;
}

void effects2d_open()
{
    int i; for(i = 0; i < EFFECTS2D_PARTICLE_COUNT; i++)
        effects2d.particles[i].animation = 0;
}

void effects2d_update()
{
    int i; for(i = 0; i < EFFECTS2D_PARTICLE_COUNT; i++)
    {
        struct effects2d_particle * p = &effects2d.particles[i];
        if(p->animation == 0)
            continue;

        struct effects2d_animation * a = &effects2d.animations[p->animation - 1];

        if(p->frame >= a->frame_count) {
            p->animation = 0;
            continue;
        }

        BMAP * tex = a->frames[(int)p->frame];

        int w = bmap_width(tex);
        int h = bmap_height(tex);

        draw_quad(
            tex,
            vector(p->x - w/2, p->y - h/2, 0),
            NULL,
            vector(w, h, 0),
            NULL,
            NULL,
            100,
            0
        );

        p->frame += a->speed * time_step;
    }
}

void effects2d_close()
{
    effects2d.particle_count = 0;
}

void effects2d_spawn(int x, int y, int type)
{
    int i; for(i = 0; i < EFFECTS2D_PARTICLE_COUNT; i++)
    {
        if(effects2d.particles[i].animation != 0)
            continue;

        effects2d.particles[i].animation = type;
        effects2d.particles[i].frame = 0;
        effects2d.particles[i].x = x;
        effects2d.particles[i].y = y;
        break;
    }
}
