
#include "camera.h"

void topdown_camera_open() {
	camera.z = 3000;
	camera.tilt = -70;
}

void topdown_camera_update() {
	/*
	bool a = (mouse_pos.x<CAMERA_MOVEBORDER);
	bool b = ((screen_size.x-mouse_pos.x)<CAMERA_MOVEBORDER);
	bool c = (mouse_pos.y<CAMERA_MOVEBORDER);
	bool d = ((screen_size.y-mouse_pos.y)<CAMERA_MOVEBORDER);
	*/
	
	bool a = false;
	bool b = false;
	bool c = false;
	bool d = false;
	
	int key_updown = (key_w-key_s) + ((int)a-(int)b);
	int key_leftright = (key_a-key_d) + ((int)c-(int)d);
	
	
	camera.x += key_updown*time_step*CAMERA_SPEED;
	camera.y += key_leftright*time_step*CAMERA_SPEED;
}