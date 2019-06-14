#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#include <acknex.h>

////////////////////////////////////////////////////////////////////////////////

//! Initialisiert das Spiel und so
void framework_init(void);

//! Aktualisiert alles.
void framework_update(void);

////////////////////////////////////////////////////////////////////////////////

//! Initialisiert eine Entity für das gegebene Subsystem
void framework_setup(ENTITY * ent, int subsystem);

//! "Friert" die Entity ein und entfernt ihre Subsystemzuweisung sowie das DYNAMIC-Flag
void framework_freeze(ENTITY * ent);

//! Löscht die angegebene Entity
void framework_remove(ENTITY * ent);

////////////////////////////////////////////////////////////////////////////////

#endif // FRAMEWORK_H
