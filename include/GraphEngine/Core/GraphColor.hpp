#ifndef __GRAPHCOLOR_HPP__
#define __GRAPHCOLOR_HPP__

#include <algorithm>

class GraphColor
{
public:
    float RED;
    float GREEN;
    float BLUE;

public:
    GraphColor()
    {
        this->RED = 0.2f + static_cast<float>(rand()) / (RAND_MAX * 1.0f);
        this->GREEN = 0.2f + static_cast<float>(rand()) / (RAND_MAX * 1.0f);
        this->BLUE = 0.2f + static_cast<float>(rand()) / (RAND_MAX * 1.0f);
    }
    GraphColor(float RED, float GREEN, float BLUE)
    {
        this->RED = RED;
        this->GREEN = GREEN;
        this->BLUE = BLUE;
    }
};

#endif