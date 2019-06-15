#ifndef PROGRESSBARS_H
#define PROGRESSBARS_H

#include "global.h"

//! renders a radial progress bar
//! @param _target Render Target
//! @param progress 0 ... 100
//! @param color foreground color
void progressbar_radial_render(BMAP * _target, var progress, var radius, COLOR * color, var alpha);

#endif // PROGRESSBARS_H
