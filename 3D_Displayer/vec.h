#ifndef VEC_H
#define VEC_H

#pragma once
#include <iostream>


using namespace std;

class vec2 {
public:
    float x, y;
    vec2(float x, float y);
    vec2();

};

/*
<< operator override for vec2 objects
*/
ostream& operator<<(std::ostream& strm, vec2 v);


class vec3 {
public:
    float x, y, z, w;
    vec3(float x, float y, float z, float w);
    vec3(float x, float y, float z);
    vec3();

    float distanceTo(vec3 other);

    vec3 operator+(vec3 a);
    vec3 operator*(float a);
    vec3 operator*(vec3 a);
    vec3 operator-(vec3 a);
    vec3 operator/(vec3 a);
    vec3 operator/(int a);
    vec3 cross(vec3 a);

    vec2 to2d(vec3 position, vec3 display_surface, float c_x, float c_y, float c_z, float s_x, float s_y, float s_z);


};

/*
<< operator override for vec3 objects
*/
ostream& operator<<(std::ostream& strm, vec3 v);


#endif