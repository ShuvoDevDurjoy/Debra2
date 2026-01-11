#ifndef __POLYGON_HPP__
#define __POLYGON_HPP__

#include "../Graphs/GraphObject.hpp"
#include "../Core/GraphApp.hpp"
#include "../../glm/glm.hpp"
#include "../../glm/gtc/matrix_transform.hpp"
#include "../../Event/KeyClicked.hpp"
#include "../../Event/MouseEvents.hpp"
#include <algorithm>

class Polygon : protected singletonGraph, public MouseEvents, public KeyClicked
{
private:
    float x = -10, y = 10, width = 20, height = 20;
    int fragments = 1000;
    std::string vertexShaderPath = "./shaders/quadratic_shaders/vertex.vs";
    std::string fragmentShaderPath = "./shaders/quadratic_shaders/fragment.fs";
    std::string geometricShaderPath = "./shaders/quadratic_shaders/geometry.gs";

    bool fill_rect = false;
    float fill_opacity = 1.0f;
    float stroke_width = 1.0f;
    float stroke_opacity = 2.0f;

    bool initialized = false;

    glm::vec3 rotationAxis;
    float rotationSpeed; // radians per second
    float startTime;

    float lastX = 0.0f, lastY = 0.0f, rotX = 0.0f, rotY = 0.0f;

    GLuint quadVAO, quadVBO;

public:
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    Vec3 cameraPos = Vec3(0.0f, 20.0f, 50.0f);
    int sides;
    float radius;

public:
    Polygon();
    Polygon(float x, float y, float width, float height)
    {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;

        generate();
    }
    Polygon(int sides, float radius);

    ~Polygon()
    {
        if (shader)
            delete shader;
        if (VAO)
            glDeleteVertexArrays(1, &VAO);
        if (VBO)
            glDeleteBuffers(1, &VBO);
    }

    void generateRegularPolygon(int sides, float radius);

    void onMouseMoveCallback(MouseEvent) override;
    void onKeyPressedOnceCallback(const KeyEvent &event) override;
    void onMouseClickCallback(MouseEvent) override {}

    void draw(float tick) override;

    void init() override;
    void initQuad();

    glm::vec3 randomUnitVector();

    void updateColorFactor();

    // Core draw function for 3D surfaces
    void drawTick(float tick);

    void generate();

    
};

#endif