#ifndef __THREEDAXES_HPP__
#define __THREEDAXES_HPP__

#include <vector>
#include <random>
#include <iostream>

#include <glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GraphEngine/Math/Vec3.hpp>
#include <GraphEngine/Core/Shader.hpp>
#include <GraphEngine/Core/GraphApp.hpp>
#include <GraphEngine/Graphs/GraphObjects/ThreeDObject.hpp>
#include <GraphEngine/Graphs/GraphObjects/GraphMathObject.hpp>

class ThreeDAxes : public ThreeDObject
{
public:
    ThreeDAxes();
    void generate(Vec3 start, Vec3 end);

    void setStrokeData() override {}
    void setFillData() override {}

    void generatePoints() override;
    void add(glm::vec3 p)
    {
        fill_points.push_back(p);
        fill_color_array.push_back(glm::vec3(0.5f, 0.5f, 0.5f)); // Default gray axes
    }
};

#endif