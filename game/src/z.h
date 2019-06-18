#ifndef Z_H
#define Z_H

var z_get(var);
bool z_pay(var,var);
bool z_isSufficient(var,var);
ENTITY* z_findNear(VECTOR* pos, var maxdist);
ENTITY* z_spawn(VECTOR* pos);
void Z();
void Z_Init();
void Z_Update();
void Z__hit(ENTITY* ptr);
void Z__wait(ENTITY* ptr);
void Z__die(ENTITY* ptr);

#endif