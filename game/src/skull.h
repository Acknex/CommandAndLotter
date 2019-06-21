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
void SKULL__fireParticle(PARTICLE *p);
void SKULL__fireEffect(PARTICLE *p);
void SKULL__smokeEffect(PARTICLE *p);
void SKULL__shootParticle(PARTICLE *p);
void SKULL__shootEffect(PARTICLE *p);
void PARTICLE_smoke(PARTICLE* p);


#endif