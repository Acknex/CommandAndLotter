#ifndef GLOBAL_H
#define GLOBAL_H

#include <acknex.h>

// boolstuff!
typedef int bool;
#define true  1
#define false 0


#define ENTITY_STATE_INACTIVE 0
#define ENTITY_STATE_WAIT_OR_WALK 1 // pathfinding (+ flocking) decide state
//#define ENTITY_STATE_WALK 2
#define ENTITY_STATE_ATTACK 3
#define ENTITY_STATE_DIE 4
#define ENTITY_STATE_DEAD 5
#define ENTITY_STATE_HIT 6


#define DAMAGE_HIT skill51
#define MAXHEALTH skill52
#define HEALTH skill53
#define ENTITY_STATE skill55
#define ENTITY_ANIM skill56
#define ENTITY_VICTIM skill57
#define ENTITY_DAMAGE skill58


#define SELCTED_SKILL skill[39]
#define UNIT_DEST_SKILL skill[40]
#define UNIT_GROUP_SKILL skill[38]

#define SK_ENTITY_JPS_POINTER_TO_UNIT_STRUCT skill97
#define jpsUnitGetFromEntity(ent) ((UNIT*)ent->SK_ENTITY_JPS_POINTER_TO_UNIT_STRUCT)

//! Markiert die Entity als "TOT" und sorgt dafür, dass sie zu Ende des Frames gelöscht wird.
#define SK_ENTITY_DEAD skill98

//! Definiert, welches Subsystem die Entity besitzt.
#define SK_SUBSYSTEM   skill99

//skill21-39: reserved for specific use
//skill40-49 reserved for shader
//skill50-67 reserved for module use

#define SUBSYSTEM_FRAMEWORK         1
#define SUBSYSTEM_CONSTRUCTION      98
#define SUBSYSTEM_SPAWNER           99
#define SUBSYSTEM_UNIT_SPUTNIK      100
#define SUBSYSTEM_UNIT_EYE          101
#define SUBSYSTEM_UNIT_SKULL        102
#define SUBSYSTEM_UNIT_LERCHE       103
#define SUBSYSTEM_UNIT_SPHERE       104
#define SUBSYSTEM_TERRAIN_SYSTEM    105
#define SUBSYSTEM_UNIT_MANAGEMENT   106
// HIER WEITERE SUBSYSTEME DEFINIEREN

#define SUBSYSTEM_LOOP(ptr,system) for(ptr = ent_next(NULL); ptr != NULL; ptr = ent_next(ptr)) if(ptr->SK_SUBSYSTEM == (system))

/* do not modify unless you really know what you do! */
#define GROUP_DEFAULT        1
#define GROUP_PLAYER_UNIT    10
#define GROUP_PLAYER_SPAWNER 11
#define GROUP_ENEMY_UNIT     12
#define GROUP_ENEMY_SPAWNER  13

#define PLAYER_ID_PLAYER 0
#define PLAYER_ID_AI 1


#define GAME_OPEN_DEBUG
#ifdef GAME_OPEN_DEBUG
	#define CONSOLE_SHOW
#endif

#endif // GLOBAL_H
