
#include "camera.h"

void topdown_camera_open() {
	camera.z = 3000;
	camera.tilt = -70;
}

void topdown_camera_update() {

	bool _left = (mouse_pos.y<CAMERA_MOVEBORDER);
	bool _right = ((screen_size.y-mouse_pos.y)<CAMERA_MOVEBORDER);
	bool _up = (mouse_pos.x<CAMERA_MOVEBORDER);
	bool _down = ((screen_size.x-mouse_pos.x)<CAMERA_MOVEBORDER);

/*
	bool _left = false;
	bool _right = false;
	bool _up = false;
	bool _down = false;
*/
	int key_updown = (key_w-key_s) + ((int)_left-(int)_right);
	int key_leftright = (key_a-key_d) + ((int)_up-(int)_down);


	//camera.x += key_updown*time_step*CAMERA_SPEED;
	//camera.y += key_leftright*time_step*CAMERA_SPEED;
	
	camera.x += (key_updown*cosv(camera.pan)- key_leftright*sinv(camera.pan))*time_step*CAMERA_SPEED;
	camera.y += (key_leftright*cosv(camera.pan)+ key_updown*sinv(camera.pan))*time_step*CAMERA_SPEED;
}
