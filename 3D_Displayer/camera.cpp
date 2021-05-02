#include "vec.h"
#include "camera.h"
#include <math.h>

Camera::Camera(vec3 position, vec3 orientation, vec3 display_surface) {
    this->position = position;
    this->orientation = orientation;
    this->display_surface = display_surface;
};

void Camera::SetRotations() {
    this->c_x = cos(orientation.x);
    this->c_y = cos(orientation.y);
    this->c_z = cos(orientation.z);

    this->s_x = sin(orientation.x);
    this->s_y = sin(orientation.y);
    this->s_z = sin(orientation.z);
}


Camera::Camera() = default;

float Camera::getDepth(vec3 a) {
    vec3 c = position;
    vec3 theta = orientation;

    float c_x = cos(theta.x);
    float c_y = cos(theta.y);
    float c_z = cos(theta.z);

    float s_x = sin(theta.x);
    float s_y = sin(theta.y);
    float s_z = sin(theta.z);

    float x = a.x - c.x;
    float y = a.y - c.y;
    float z = a.z - c.z;

    return c_x * (c_y * z + s_y * (s_z * y + c_z * x)) - s_x * (c_z * y - s_z * x);
}