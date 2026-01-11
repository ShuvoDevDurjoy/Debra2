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
#include "GraphObjects/GraphMathObject.hpp"
class ThreeDAxes : public GraphMathObject
{

private:
    std::string vertexShaderPath = "./shaders/surface_shaders/vertex.vs";
    std::string fragmentShaderPath = "./shaders/surface_shaders/fragment.fs";

    Shader *shader;
    GLuint VAO, VBO;

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
    std::vector<glm::vec3> points;

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

    void Init(float) override;
    void draw(float tick) override
    {
        drawTick(tick);
    }

    // Core draw function for 3D surfaces
    void drawTick(float tick);

    void generate(Vec3 start, Vec3 end);

    int getSize() { return points.size(); }

    void add(glm::vec3 p){
        points.push_back(p);
    }
    // Core draw function for 3D surfaces
};

#endif