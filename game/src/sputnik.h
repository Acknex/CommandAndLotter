#ifndef SPUTNIK_H
#define SPUTNIK_H

void Sputnik();
void SPUTNIK_Init();
void SPUTNIK_Update();

// local
void SPUTNIK__wait(ENTITY* ptr);
void SPUTNIK__walk(ENTITY* ptr);
void SPUTNIK__attack(ENTITY* ptr);
void SPUTNIK__die(ENTITY* ptr);
void SPUTNIK__hit(ENTITY* ptr);
void SPUTNIK__hitcheck(ENTITY* ptr);


#endif