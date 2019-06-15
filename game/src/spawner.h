#ifndef SPAWNER_H
#define SPAWNER_H

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