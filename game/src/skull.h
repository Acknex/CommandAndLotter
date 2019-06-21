#ifndef SKULL_H
#define SKULL_H

void Skull();
void SKULL_Init();
void SKULL_Update();

// local
void SKULL__wait_or_walk(ENTITY * ptr);
void SKULL__attack(ENTITY* ptr);
void SKULL__die(ENTITY* ptr);
void SKULL__hit(ENTITY* ptr);


#endif