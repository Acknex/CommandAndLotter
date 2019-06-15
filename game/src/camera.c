
#include "camera.h"

void topdown_camera_open() 
{
	camera.z = 3000;
	camera.tilt = -70;
	topdown_camera_height = 4;
	
	vec_set(&topdown_camera_center, nullvector);
}

void topdown_camera_update() 
{
	if(key_p)
		topdown_camera_set_pos(nullvector);
	
#ifdef DEBUG
	if(def_camera == 0)
#endif
	{
		bool _left = (mouse_pos.y<CAMERA_MOVEBORDER);
		bool _right = ((screen_size.y-mouse_pos.y)<CAMERA_MOVEBORDER);
		bool _up = (mouse_pos.x<CAMERA_MOVEBORDER);
		bool _down = ((screen_size.x-mouse_pos.x)<CAMERA_MOVEBORDER);
		
		int key_updown = (key_w-key_s) + ((int)_left-(int)_right);
		int key_leftright = (key_a-key_d) + ((int)_up-(int)_down);
	
		topdown_camera_center.x += (key_updown*cosv(camera.pan)- key_leftright*sinv(camera.pan))*time_step*CAMERA_SPEED;
		topdown_camera_center.y += (key_leftright*cosv(camera.pan)+ key_updown*sinv(camera.pan))*time_step*CAMERA_SPEED;
		topdown_camera_center.z = 0;
		
		topdown_camera_height += -0.1*(topdown_camera_height/10)*mickey.z*time_step;
		topdown_camera_height = clamp(topdown_camera_height, 1, 10.);
		
		
		VECTOR offset;
		vec_set(&offset, nullvector);
		offset.z = -sinv(camera.tilt)*CAMERA_HEIGHT*topdown_camera_height;
		
		offset.x = -cosv(camera.tilt)*CAMERA_HEIGHT*topdown_camera_height*cosv(camera.pan);
		offset.y = -cosv(camera.tilt)*CAMERA_HEIGHT*topdown_camera_height*sinv(camera.pan);
		
		vec_set(&camera.x, vec_add(offset, topdown_camera_center));
	}
}


void topdown_camera_set_pos(VECTOR* target_pos)
{
	vec_set(&topdown_camera_center, target_pos);
}