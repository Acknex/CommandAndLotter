#ifndef BRIEFING_H
#define BRIEFING_H

#include "global.h"

#define BRIEFING_PENDING 0 //< status pending
#define BRIEFING_ACCEPT  1 //< mission accepted
#define BRIEFING_ABORT   2 //< cancelled

void briefing_init(void);
void briefing_open(void);
void briefing_update(void);
void briefing_close(void);
int briefing_getResult(void);

#endif // BRIEFING_H
