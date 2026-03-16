#pragma once

#include <cmath>
#include <vector>
#include <glad.h>
#include <glfw3.h>
#include <cstdarg>
#include <climits>
#include <iostream>
#include <algorithm>
#include <glm/glm.hpp>
#include <GraphEngine/Math/Var.hpp>
#include <GraphEngine/Core/Shader.hpp>
#include <GraphEngine/Camera/Camera.hpp>
#include <GraphEngine/Rendering/Font.hpp>
#include <GraphEngine/Utils/GraphColor.hpp>
#include <GraphEngine/Utils/AnimationMode.hpp>
#include <GraphEngine/Animations/Animation.hpp>
#include <GraphEngine/Animations/CameraAnimation.hpp>
#include <GraphEngine/Animations/AnimationManager.hpp>

class GraphApp;
class Animation;
class GraphObject;
class GraphMathObject;
class CameraAnimation;

class Scene
{

public:
    enum class ControlMode
    {
        CAMERA_CONTROL,  // Keyboard: F/B zoom, arrows pan, R reset
        TOUCHPAD_CONTROL // Mouse drag: left drag to pan, scroll to zoom
    };

private:
    bool scene_instantiated = false;

private:
    float time_stamp = 0.0f;
    ControlMode controlMode;

private:
    std::vector<GraphMathObject *> graphs;
    std::vector<Animation *> animations;
    std::vector<CameraAnimation *> camera_animations;
    std::vector<GraphMathObject *> flat_scene_registry;

private:
    GraphApp *app;
    GLFWwindow *window;
    Camera *camera;

    friend class GraphApp;

protected:
    Scene(int width = 1200, int height = 600, ControlMode control_mode = ControlMode::TOUCHPAD_CONTROL);

private:
    static void frame_size_buffer_callback(GLFWwindow *, int, int);

    void draw(float);

public:
    // --- Singleton Access Point ---
    ~Scene() {};

    void InitGraphs();

    bool scene_exists()
    {
        return scene_instantiated;
    }
    ControlMode getControlMode() const { return controlMode; }
    Camera *getCamera() { return camera; }
    void timeProgress(float t)
    {
        this->time_stamp += t;
        this->time_stamp = std::max(0.0f, this->time_stamp);
    }
    void run();

public:
    void register_object(GraphMathObject *obj);
    void play(Animation *animation, float duration = 2.0f);
    void play(std::vector<Animation *> anims, float duration = 2.0f);
    void play(CameraAnimation *camera_animation, float start_time, float duration);
    void add(GraphMathObject *graph);
};