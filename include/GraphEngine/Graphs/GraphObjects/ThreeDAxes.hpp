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

struct TConfig{
    std::pair<float, float> x_range = {-10.0f, 10.0f};
    std::pair<float, float> y_range = {-10.0f, 10.0f};
    std::pair<float, float> z_range = {-10.0f, 10.0f};
    float shaft_radius = 0.15f;
    float head_length = 1.0f;
    GraphColor axis_color = GraphColor(1, 1, 0);
    float tick_dis = 2.0f;
    float tick_height = 0.5f;
    float tick_width = 0.5f;
    float axis_label_size = 2.0f;
    float label_font_size = 1.5f;
};

class ThreeDAxes : public ThreeDObject
{
public:
    std::pair<float, float> x_range = {-5.0f, 5.0f};
    std::pair<float, float> y_range = {-5.0f, 5.0f};
    std::pair<float, float> z_range = {-5.0f, 5.0f};
    TConfig config;

public:
    ThreeDAxes(TConfig config);
    void setXRange(float min, float max) { x_range = {min, max}; }
    void setYRange(float min, float max) { y_range = {min, max}; }
    void setZRange(float min, float max) { z_range = {min, max}; }

    void generatePoints() override;
    void Init() override;

    using ThreeDObject::add;
};

#endif