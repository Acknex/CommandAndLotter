#ifndef EFFECTS2D_H
#define EFFECTS2D_H

#define EFFECTS2D_TYPE_GOTO   1
#define EFFECTS2D_TYPE_ATTACK 2
#define EFFECTS2D_TYPE_MINE   3

void effects2d_init(void);
void effects2d_open(void);
void effects2d_update(void);
void effects2d_close(void);

void effects2d_spawn(VECTOR * worldpos, int type);

#endif // EFFECTS2D_H
