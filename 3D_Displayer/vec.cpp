#include <SFML/Graphics.hpp>
#include <iostream>
#include "vec.h"
#include <math.h>

using namespace std;


vec2::vec2(float x, float y) {
    this->x = x;
    this->y = y;
};
vec2::vec2() = default;

    //sf::Vector2f toVector2f() {
    //    return sf::Vector2f(this->x, this->y);
    //}



/*
<< operator override for vec2 objects
*/
ostream& operator<<(std::ostream& strm, vec2 v) {
    return strm << "(" << v.x << ", " << v.y << ")";
}


vec3::vec3(float x, float y, float z, float w) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
};
vec3::vec3(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = 0.f;
};
vec3::vec3() = default;


float vec3::distanceTo(vec3 other) {
    float d = pow(this->x - other.x, 2) + pow(this->y - other.y, 2) + pow(this->z - other.z, 2);
    return sqrt(d);
}


vec3 vec3::cross(vec3 a) {
    return vec3(
        a.y * z - a.z * y,
        a.z * x - a.x * z,
        a.x * y - a.y * x
    );

};



vec3 vec3::operator+(vec3 a) {
    vec3 r = a;
    r.x += this->x;
    r.y += this->y;
    r.z += this->z;
    r.w += this->w;
    return r;
};

vec3 vec3::operator-(vec3 a) {
    vec3 r = a;
    r.x -= this->x;
    r.y -= this->y;
    r.z -= this->z;
    r.w -= this->w;
    return r;
};
vec3 vec3::operator*(vec3 a) {
    vec3 r = a;
    r.x *= this->x;
    r.y *= this->y;
    r.z *= this->z;
    r.w *= this->w;
    return r;
};
vec3 vec3::operator*(float a) {
    vec3 r = vec3(x, y, z);
    r.x = this->x * a;
    r.y = this->y * a;
    r.z = this->z * a;
    r.w = this->w * a;
    return r;
};

vec3 vec3::operator/(vec3 a) {
    vec3 r = vec3(0, 0, 0);
    r.x = this->x / a.x;
    r.y = this->y / a.y;
    r.z = this->z / a.z;
    r.w = this->w / a.w;
    return r;
};

vec3 vec3::operator/(int a) {
    vec3 r = vec3(0, 0, 0);
    r.x = this->x / a;
    r.y = this->y / a;
    r.z = this->z / a;
    r.w = this->w / a;
    return r;
};

vec2 vec3::to2d(vec3 position, vec3 display_surface, float c_x, float c_y, float c_z, float s_x, float s_y, float s_z) {
    vec3 c = position;
    vec3 e = display_surface;

    float x = this->x - c.x;
    float y = this->y - c.y;
    float z = this->z - c.z;

    vec3 d(
        c_y * (s_z * y + c_z * x) - s_y * z,
        s_x * (c_y * z + s_y * (s_z * y + c_z * x)) + c_x * (c_z * y - s_z * x),
        c_x * (c_y * z + s_y * (s_z * y + c_z * x)) - s_x * (c_z * y - s_z * x)
    );

    vec2 b(
        (e.z / d.z) * d.x + e.x,
        (e.z / d.z) * d.y + e.y
    );

    return b;
}
/*
<< operator override for vec3 objects
*/
ostream& operator<<(std::ostream& strm, vec3 v) {
    return strm << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}