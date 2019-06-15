#ifndef CAMERA_H
#define CAMERA_H

#define CAMERA_SPEED 100
#define CAMERA_ROTATIONSPEED 100
#define CAMERA_SCALESPEED 0.015
#define CAMERA_MOVEBORDER 10
#define CAMERA_HEIGHT 1000

void topdown_camera_open();
void topdown_camera_update();

void topdown_camera_set_pos(VECTOR*);

VECTOR *get_pos_under_cursor();
 
#endif CAMERA_H