#ifndef Z_H
#define Z_H

var z_get();
ENTITY* z_spawn(VECTOR* pos);
void Z();
void Z_Init();
void Z_Update();
void Z__hit(ENTITY* ptr);
void Z__wait(ENTITY* ptr);
void Z__die(ENTITY* ptr);

#endif