#ifndef CAMERA_H
#define CAMERA_H

#define CAMERA_SPEED 100
#define CAMERA_MOVEBORDER 10
#define CAMERA_HEIGHT 1000

VIEW *topdown_camera;

void topdown_camera_init();
void topdown_camera_update();
 
#endif CAMERA_H