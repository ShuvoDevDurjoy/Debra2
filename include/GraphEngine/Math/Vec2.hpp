#ifndef __VEC2_HPP__
#define __VEC2_HPP__

struct Vec2
{
    float x = 0.0f;
    float y = 0.0f;

    Vec2() {}

    Vec2(float x, float y)
    {
        this->x = x;
        this->y = y;
    }
};

#endif