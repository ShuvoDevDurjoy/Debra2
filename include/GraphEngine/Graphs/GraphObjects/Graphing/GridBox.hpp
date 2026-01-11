#ifndef __GRIDBOX_HPP__
#define __GRIDBOX_HPP__

#include "../GraphObject.hpp"
#include "../GraphGroup.hpp"
#include "Line.hpp"

class GridBox : public GraphObject
{
public:
    int row = 3, column = 3;

public:
    GridBox(int row = 3, int column = 3);

    void generateGrid();

    void setDimension(float x = -15, float y = -15, float width = 30, float height = 30);
    void generateBox();
};

#endif