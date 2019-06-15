#ifndef UNIT_H
#define UNIT_H

#define UNIT_TARGET skill30
#define UNIT_TARGETX skill30
#define UNIT_TARGETY skill31
#define UNIT_TARGETZ skill32

var unit_setTarget(ENTITY* ent, VECTOR* pos);
ENTITY* unit_spawn(int unittype, VECTOR* pos);

#endif