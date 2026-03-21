#ifndef __RECTANGLE_SHAPE_HPP__
#define __RECTANGLE_SHAPE_HPP__

#include <GraphEngine/Graphs/GraphObjects/GraphObject.hpp>

class Rectangle : public GraphObject
{
public:
    float width, height, x, y;

public:
    Rectangle(float width = 10.0f, float height = 10.0f, float x = 0, float y = 0)
        : width(width), height(height), x(x), y(y)
    {
        Init();
    }

    void generatePoints() override
    {
        std::cout << "This is adding points for rect" << std::endl;
        float h_w = width * 0.5f;
        float h_h = height * 0.5f;

        start_bezier_path(glm::vec3(x - h_w, y + h_h, 0));
        add_line_to(glm::vec3(x + h_w, y + h_h, 0));
        add_line_to(glm::vec3(x + h_w, y - h_h, 0));
        add_line_to(glm::vec3(x - h_w, y - h_h, 0));
        add_line_to(glm::vec3(x - h_w, y + h_h, 0));

        setDimension(x - h_w, x + h_w, y - h_h, y + h_h);
    }
};

class Square : public Rectangle
{
public:
    Square(float side = 5.0f, float cx = 0.0f, float cy = 0.0f)
        : Rectangle(side, side, cx, cy) {}
};

#endif
