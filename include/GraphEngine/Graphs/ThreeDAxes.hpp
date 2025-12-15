#ifndef __THREEDAXES_HPP__
#define __THREEDAXES_HPP__

#include "GraphObject.hpp"
#include "../Core/Shader.hpp"
#include "../Core/GraphApp.hpp"
#include "../Math/Vec3.hpp"
#include <vector>
#include <iostream>
#include "../../glad.h"
#include "../../glm/glm.hpp"
#include "../../glm/gtc/matrix_transform.hpp"
#include "../../Event/MouseEvents.hpp"
#include "../../Event/KeyClicked.hpp"
#include "Assets/Arrow.hpp"
#include <random>

class ThreeDAxes : public singletonGraph, public MouseEvents, public KeyClicked
{

private:
    std::string vertexShaderPath = "./shaders/surface_shaders/vertex.vs";
    std::string fragmentShaderPath = "./shaders/surface_shaders/fragment.fs";

    int count = 0;
    int total_size;

    GraphColor color;

    bool initialized = false;
    float lastX = 0.0f, lastY = 0.0f, rotX = 0.0f, rotY = 0.0f;

public:
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    Vec3 cameraPos = Vec3(0.0f, 20.0f, 50.0f);


public:
    ThreeDAxes();

    ~ThreeDAxes(){
        if (shader)
            delete shader;
        if (VAO)
            glDeleteVertexArrays(1, &VAO);
        if (VBO)
            glDeleteBuffers(1, &VBO);
    }

    void onMouseMoveCallback(MouseEvent) override;
    void onKeyPressedOnceCallback(const KeyEvent &event) override;
    void onMouseClickCallback(MouseEvent) override{}

    void draw(float tick) override
    {
        drawTick(tick);
    }

    // Core draw function for 3D surfaces
    void drawTick(float tick) override;

    void init() override;

    void generate(Vec3 start, Vec3 end);
    // Core draw function for 3D surfaces
};

#endif