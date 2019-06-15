#ifndef MATERIALS_H
#define MATERIALS_H

#define PRAGMA_BIND "default.fx"

BMAP *bmapPPOriginal = NULL;

MATERIAL *matPPFilter =
{
    effect = "pp_filter.fx";
}

MATERIAL *matPPBlurVertical =
{
	effect = "pp_blur.fx";
}

MATERIAL *matPPBlurHorizontal =
{
	effect = "pp_blur.fx";
}

MATERIAL *matPPCombine =
{
	effect = "pp_combine.fx";
}

VIEW* pp_laststage(VIEW* view)
{
    if(!view) view = camera;
    while(view->stage) view = view->stage;
    return view;
}

VIEW* pp_findstage(VIEW* view)
{
    VIEW *v;
    for(v = ptr_first(camera); v; v = v->link.next)
        if(view == v->stage) return v;
    return camera;
}

VIEW* pp_stage = NULL;
VIEW* pp_view = NULL;

function pp_event() { return; }

function pp_set(VIEW* view,MATERIAL* m)
{
    if(!pp_view) pp_view = camera;
    if(!view) view = camera;
    if(m) {
// store original view
      pp_view = view;
// create new view stage for postprocessing
        if(!pp_stage) pp_stage = view_create(-1);
        pp_stage->material = m;
        pp_stage->flags |= PROCESS_TARGET;
        pp_stage->size_x = view->size_x;
        pp_stage->size_y = view->size_y;
        view->stage = pp_stage;
// set up material skill default values
        if(m->skill11) m->skill1 = floatv(m->skill11);
        if(m->skill12) m->skill2 = floatv(m->skill12);
        if(m->skill13) m->skill3 = floatv(m->skill13);
        if(m->skill14) m->skill4 = floatv(m->skill14);
// make sure material event is executed before the next stage is assigned
        if(m->event) { pp_event = m->event; pp_event(); }
    } else {
        view->stage = NULL;
        if(pp_stage) pp_stage->material = NULL;
    }
}

function pp_add(MATERIAL* m)
{
    if(!pp_view) pp_view = camera;
    if(!pp_stage) pp_stage = camera;
    if(m) {
        VIEW* view = pp_laststage(pp_stage);
        pp_stage = NULL;
        pp_set(view,m);
    } else {
        ptr_remove(pp_stage);
        pp_stage = pp_view;
        pp_stage->stage = NULL;
        pp_view = pp_findstage(pp_stage);
    }
}

void UpdateRenderTargets()
{
	if(bmapPPOriginal)
	{
		if(bmap_width(bmapPPOriginal) == screen_size.x && bmap_height(bmapPPOriginal) == screen_size.y)
		{
			return;
		}
	}
	
	if(bmapPPOriginal)
		bmap_remove(bmapPPOriginal);

	bmapPPOriginal = bmap_createblack(screen_size.x, screen_size.y, 8888);
	
    camera.bmap = bmapPPOriginal;
    matPPCombine.skin1 = bmapPPOriginal;

    VIEW * it = camera;
    while(it != NULL)
    {
        it->size_x = screen_size.x;
        it->size_y = screen_size.y;
        it = it->stage;
    }
}

void materials_reinit()
{
    while(pp_view != NULL && pp_view != camera)
    {
        pp_add(NULL);
    }

    UpdateRenderTargets();

    matPPFilter.skill1 = floatv(0.7);

    matPPBlurHorizontal.skill1 = floatv(2.0);
    matPPBlurHorizontal.skill2 = floatv(0.0);

    matPPBlurVertical.skill1 = floatv(0.0);
    matPPBlurVertical.skill2 = floatv(2.0);

    matPPCombine.skill1 = floatv(1.5);

    pp_add(matPPFilter);
    pp_add(matPPBlurVertical);
    pp_add(matPPBlurHorizontal);
    pp_add(matPPCombine);
}

int AmazingRendertargetCounter = 0;
void rendertargetUpdateBlubb()
{
    AmazingRendertargetCounter = 1;
}

void updateRenderTargetsIfNeeded()
{
    if(AmazingRendertargetCounter == 1)
    {
        materials_reinit();
    }

    AmazingRendertargetCounter = 0;
}

void SetupPostprocessing()
{
    materials_reinit();

    on_d3d_reset = rendertargetUpdateBlubb;
}

#endif
