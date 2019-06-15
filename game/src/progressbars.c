#include "progressbars.h"

MATERIAL * progressbar_radial_mtl =
{
    effect = "progress-radial.fx";
    flags = AUTORELOAD;
}

void progressbar_radial_render(BMAP *_target, var progress, COLOR *color, var alpha)
{
    progressbar_radial_mtl->skill1 = floatv(color.red);
    progressbar_radial_mtl->skill2 = floatv(color.green);
    progressbar_radial_mtl->skill3 = floatv(color.blue);
    progressbar_radial_mtl->skill4 = floatv(alpha / 100.0);
    progressbar_radial_mtl->skill5 = floatv(progress / 100.0);
    bmap_process(_target, NULL, progressbar_radial_mtl);
}
