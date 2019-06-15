#ifndef SPAWNER_H
#define SPAWNER_H

#define SPAWNER_ENEMY 0
#define SPAWNER_PLAYER 1

ENTITY* spawner_spawn(int spawnertype, VECTOR* pos, var owner);
var spawner_produce(ENTITY* ent);
var spawner_getQueue(ENTITY* ent);
var spawner_getProgress(ENTITY* ent);
var spawner_getHealth(ENTITY* ent);


void Spawner();
void SPAWNER_Init();
void SPAWNER_Update();

/* internal */
void SPAWNER__hit(ENTITY* ptr);
void SPAWNER__hitcheck(ENTITY* ptr);
void SPAWNER__construct(ENTITY* ptr);
void SPAWNER__active(ENTITY* ptr);
void SPAWNER__produce(ENTITY* ptr);
void SPAWNER__die(ENTITY* ptr);

#endif