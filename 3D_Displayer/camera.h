#ifndef CAMERA_H
#define CAMERA_H

#pragma once
#include "vec.h"

using namespace std;

class Camera {
public:
    vec3 position;
    vec3 orientation;
    vec3 display_surface;

    float c_x;
    float c_y;
    float c_z;
    float s_x;
    float s_y;
    float s_z;

    Camera(vec3 position, vec3 orientation, vec3 display_surface);

    Camera();
    
    void SetRotations();

    float getDepth(vec3 a);
};

#endif