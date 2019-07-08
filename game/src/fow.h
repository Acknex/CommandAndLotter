#ifndef FOW_H
#define FOW_H

//#define USE_FOW

#define FOW_SCOUTED 0
#define FOW_HIDDEN 1
#define FOW_BORDER 2

#define FOW_SIGHT_RANGE 1500


void fow_open();
void fow_update();
void fov_uncover(VECTOR *pos, var range);
void fow_close(void);

void fow_uncoverTile(TILE *);
bool fow_isVisible(TILE *);
bool fow_isPosVisible(VECTOR *);

#endif
