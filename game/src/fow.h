#ifndef FOW_H
#define FOW_H

#define USE_FOW

#define FOW_HIDDEN 0
#define FOW_SCOUTED 1
#define FOW_SIGHT_RANGE 4


void fow_open();
void fow_update();
void fov_uncover(VECTOR *pos, var range);

#endif