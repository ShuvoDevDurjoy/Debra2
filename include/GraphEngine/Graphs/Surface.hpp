#ifndef __SURFACE_HPP__
#define __SURFACE_HPP__

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

class Surface : public singletonGraph,public MouseEvents
{
private:
    std::string vertexShaderPath = "./shaders/surface_shaders/vertex.vs";
    std::string fragmentShaderPath = "./shaders/surface_shaders/fragment.fs";

    GraphColor color;

    bool initialized = false;

public:
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    Vec3 cameraPos;

public:
    Surface();

    ~Surface()
    {
        if (shader)
            delete shader;
        if (VAO)
            glDeleteVertexArrays(1, &VAO);
        if (VBO)
            glDeleteBuffers(1, &VBO);
    }

    void onMouseMoveCallback(MouseEvent) override;

    // Override draw to accept tick parameter
    void draw(float tick) override
    {
        drawTick();
    }

    void init() override;

    // Core draw function for 3D surfaces
    void drawTick();
};

#endif
