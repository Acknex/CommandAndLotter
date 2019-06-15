
#include "camera.h"

var topdown_camera_height;
VECTOR topdown_camera_center;
VECTOR topdown_camera_centerTarget;

void topdown_camera_open() 
{
	camera.z = 3000;
	camera.tilt = -70;
	topdown_camera_height = 4;
	
	vec_set(&topdown_camera_center, nullvector);
}

void topdown_camera_update() 
{
	
#ifdef DEBUG
	if(def_camera == 0)
#endif
	{
	//if(key_p)
	//	topdown_camera_set_pos(nullvector);
		
		if(mouse_middle || key_q)
		{
			camera.pan += CAMERA_ROTATIONSPEED * mickey.x / screen_size.x;
			camera.tilt += CAMERA_ROTATIONSPEED * mickey.y / screen_size.y;
			camera.tilt = clamp(camera.tilt, -90, -30);
		}
	
		bool _left = (mouse_pos.y<CAMERA_MOVEBORDER);
		bool _right = ((screen_size.y-mouse_pos.y)<CAMERA_MOVEBORDER);
		bool _up = (mouse_pos.x<CAMERA_MOVEBORDER);
		bool _down = ((screen_size.x-mouse_pos.x)<CAMERA_MOVEBORDER);
		
		int key_updown = (key_w-key_s) + ((int)_left-(int)_right);
		int key_leftright = (key_a-key_d) + ((int)_up-(int)_down);
	
		topdown_camera_centerTarget.x += (key_updown*cosv(camera.pan)- key_leftright*sinv(camera.pan))*time_step*CAMERA_SPEED;
		topdown_camera_centerTarget.y += (key_leftright*cosv(camera.pan)+ key_updown*sinv(camera.pan))*time_step*CAMERA_SPEED;
		topdown_camera_centerTarget.z = 0;
		
		topdown_camera_height += -0.1*(topdown_camera_height/10.)*mickey.z*time_step;
		topdown_camera_height = clamp(topdown_camera_height, 1, 10.);
		
		
		
		VECTOR movement;
		vec_set(&movement, &topdown_camera_centerTarget);
		vec_sub(&movement, &topdown_camera_center);
		vec_scale(&movement, time_step);
		
		vec_add(&topdown_camera_center, &movement);
		
		
		
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
	vec_set(&topdown_camera_centerTarget, target_pos);
}


VECTOR *get_pos_under_cursor()
{
	VECTOR ray;
	ray.x = mouse_pos.x;
	ray.y = mouse_pos.y;
	ray.z = camera.clip_far;
	vec_for_screen(ray, camera);
	
	return maploader_trace(&camera->x, ray);
}