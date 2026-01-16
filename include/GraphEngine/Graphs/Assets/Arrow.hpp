// #ifndef __ARROW_HPP__
// #define __ARROW_HPP__

// #include "../GraphObject.hpp"
// #include "../../../Event/MouseEvents.hpp"
// #include "../../../Event/KeyClicked.hpp"
// #include "../../Core/Shader.hpp"
// #include "../../Core/GraphApp.hpp"
// #include "../../Math/Vec3.hpp"
// #include <vector>
// #include <iostream>
// #include "../../../glad.h"
// #include "../../../glm/glm.hpp"
// #include "../../../glm/gtc/matrix_transform.hpp"
// #include <random>

// class Arrow3D : public singletonGraph, public KeyClicked, public MouseEvents
// {
// private:
//     std::string vertexShaderPath = "./shaders/surface_shaders/vertex.vs";
//     std::string fragmentShaderPath = "./shaders/surface_shaders/fragment.fs";

//     GraphColor color;

//     bool initialized = false;

//     glm::vec3 rotationAxis;
//     float rotationSpeed; // radians per second
//     float startTime;

//     float lastX = 0.0f, lastY = 0.0f, rotX = 0.0f, rotY = 0.0f;

// public:
//     glm::mat4 model = glm::mat4(1.0f);
//     glm::mat4 view = glm::mat4(1.0f);
//     glm::mat4 projection = glm::mat4(1.0f);

//     Vec3 cameraPos = Vec3(0.0f, 20.0f, 50.0f);
// private: 
//     Vec3 start;
//     Vec3 end;
    

// public:
//     Arrow3D();

//     Arrow3D(Vec3 start, Vec3 end);

//     void generate();

//     ~Arrow3D(){
//         if (shader)
//             delete shader;
//         if (VAO)
//             glDeleteVertexArrays(1, &VAO);
//         if (VBO)
//             glDeleteBuffers(1, &VBO);
//     }

//     void onMouseMoveCallback(MouseEvent) override;
//     void onKeyPressedOnceCallback(const KeyEvent &event) override;
//     void onMouseClickCallback(MouseEvent) override{}

//     std::vector<float>& getPoints(){
//         return points;
//     }

//     void init() override;

//     // Override draw to accept tick parameter
//     void draw(float tick) override
//     {
//         drawTick(tick);
//     }

//     void drawTick(float tick);
// };

// #endif