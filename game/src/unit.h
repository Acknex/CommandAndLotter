#ifndef UNIT_H
#define UNIT_H

#define UNIT_TARGET skill30
#define UNIT_TARGETX skill30
#define UNIT_TARGETY skill31
#define UNIT_TARGETZ skill32

//#define UNIT_ENEMY 0
//#define UNIT_PLAYER 1

var unit_setTarget(ENTITY* ent, VECTOR* pos);
var unit_setVictim(ENTITY* ent, ENTITY* victim);
ENTITY* unit_spawn(int unittype, VECTOR* pos, var owner);
ENTITY* unit_spawn(int unittype, VECTOR* pos, VECTOR* targetPos, var owner);
VECTOR* unit_getTarget(ENTITY* ent);
ENTITY* unit_getVictim(ENTITY* ent);
var unit_getHealth(ENTITY* ent);
var unit_setDamage(ENTITY* victim, ENTITY* attacker);
int unit_getType(ENTITY* ent);
ENTITY* unit_findNextVictim(ENTITY* ptr);
ENTITY* unit_findNextVictim(ENTITY* ptr, var unittype);
void unit_deactivate(ENTITY* ent);


#endif