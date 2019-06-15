#ifndef CAMERA_H
#define CAMERA_H

#define CAMERA_SPEED 100
#define CAMERA_MOVEBORDER 10
#define CAMERA_HEIGHT 1000

var topdown_camera_height;
VECTOR topdown_camera_center;

void topdown_camera_open();
void topdown_camera_update();
 
#endif CAMERA_H