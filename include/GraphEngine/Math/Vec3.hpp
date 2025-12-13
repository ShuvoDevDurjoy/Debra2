#ifndef __VEC3_HPP__
#define __VEC3_HPP__

struct Vec3{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vec3(){}

    Vec3(float x, float y, float z){
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

#endif