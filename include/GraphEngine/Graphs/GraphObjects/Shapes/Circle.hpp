#ifndef __CIRCLE_HPP__
#define __CIRCLE_HPP__

#include "BasePolygon.hpp"

class Circle: public BasePolygon{
    public: 
        Circle(float radius = 5, float centerX = 0, float centerY = 0)
            : BasePolygon(32, radius, centerX, centerY) {
        }

};

class Rectangle: public BasePolygon{
    public: 
        Rectangle(float side_length = 10, float x = -10, float y = 10): BasePolygon(4, side_length, x + side_length, y - side_length ){}
};

class Dot: public BasePolygon{
    public:
        Dot(float centerX = 0, float centerY = 0, float radius = 0.5f, float res = 32) : BasePolygon(res, radius, centerX, centerY){}
};

#endif