#ifndef GLOBAL_H
#define GLOBAL_H

// boolstuff!
typedef int bool;
#define true  1
#define false 0

//! Markiert die Entity als "TOT" und sorgt dafür, dass sie zu Ende des Frames gelöscht wird.
#define SK_ENTITY_DEAD skill98

//! Definiert, welches Subsystem die Entity besitzt.
#define SK_SUBSYSTEM   skill99

//skill21-39: reserved for specific use
//skill40-49 reserved for shader
//skill50-67 reserved for module use

#define SUBSYSTEM_FRAMEWORK         1
// HIER WEITERE SUBSYSTEME DEFINIEREN

#define SUBSYSTEM_LOOP(ptr,system) for(ptr = ent_next(NULL); ptr != NULL; ptr = ent_next(ptr)) if(ptr->SK_SUBSYSTEM == (system))

/* do not modify unless you really know what you do! */
// #define GROUP_PLAYER 3
// #define GROUP_ENEMY 10

#endif // GLOBAL_H
