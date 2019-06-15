#include "effects2d.h"
#include <acknex.h>

#define EFFECTS2D_PARTICLE_COUNT 50

struct effects2d_animation
{
    BMAP * frames[10];
    int frame_count;
    var speed;
    var size;
};

struct effects2d_particle
{
    VECTOR pos;
    int animation;
    var frame;
};

struct effects2d_t
{
    struct effects2d_animation animations[3];

    struct effects2d_particle particles[50];
};

struct effects2d_t effects2d;

void effects2d_init()
{
    // GOTO
    effects2d.animations[0].frame_count = 5;
    effects2d.animations[0].speed = 1;
    effects2d.animations[0].size = 0.5;
    effects2d.animations[0].frames[0] = bmap_create("Cursor1.png");
    effects2d.animations[0].frames[1] = bmap_create("Cursor2.png");
    effects2d.animations[0].frames[2] = bmap_create("Cursor3.png");
    effects2d.animations[0].frames[3] = bmap_create("Cursor4.png");
    effects2d.animations[0].frames[4] = bmap_create("Cursor5.png");

    // ATTACK
    effects2d.animations[1].frame_count = 5;
    effects2d.animations[1].speed = 1;
    effects2d.animations[1].size = 0.5;
    effects2d.animations[1].frames[0] = bmap_create("Cursor1.png");
    effects2d.animations[1].frames[1] = bmap_create("Cursor2.png");
    effects2d.animations[1].frames[2] = bmap_create("Cursor3.png");
    effects2d.animations[1].frames[3] = bmap_create("Cursor4.png");
    effects2d.animations[1].frames[4] = bmap_create("Cursor5.png");

    // MINE
    effects2d.animations[2].frame_count = 5;
    effects2d.animations[2].speed = 1;
    effects2d.animations[2].size = 0.5;
    effects2d.animations[2].frames[0] = bmap_create("Cursor1.png");
    effects2d.animations[2].frames[1] = bmap_create("Cursor2.png");
    effects2d.animations[2].frames[2] = bmap_create("Cursor3.png");
    effects2d.animations[2].frames[3] = bmap_create("Cursor4.png");
    effects2d.animations[2].frames[4] = bmap_create("Cursor5.png");
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

        VECTOR pos;
        vec_set(pos, &p->pos);
        if(vec_to_screen(pos, camera))
        {
            draw_quad(
                tex,
                vector(pos.x - a->size * w/2, pos.y - a->size * h/2, 0),
                NULL,
                vector(w, h, 0),
                vector(a->size, a->size, 0),
                NULL,
                80,
                0
            );
        }

        p->frame += a->speed * time_step;
    }
}

void effects2d_close()
{

}

void effects2d_spawn(VECTOR * worldpos, int type)
{
    int i; for(i = 0; i < EFFECTS2D_PARTICLE_COUNT; i++)
    {
        if(effects2d.particles[i].animation != 0)
            continue;

        effects2d.particles[i].animation = type;
        effects2d.particles[i].frame = 0;
        // effects2d.particles[i].x = x;
        vec_set(&effects2d.particles[i].pos, worldpos);
        break;
    }
}
