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
#include <GraphEngine/Utils/GraphColor.hpp>
#include <GraphEngine/Utils/AnimationMode.hpp>
#include <GraphEngine/Animations/Animation.hpp>
#include <GraphEngine/Animations/CameraAnimation.hpp>
#include <GraphEngine/Animations/AnimationManager.hpp>

#include <GraphEngine/Scene/ControlPolicies.hpp>

class GraphApp;
class Animation;
class GraphObject;
class GraphMathObject;
class CameraAnimation;

/**
 * Base Scene class handling common state and rendering.
 */
class Scene
{
private:
    bool scene_instantiated = false;

protected:
    float time_stamp = 0.0f;
    std::vector<GraphMathObject *> graphs;
    std::vector<Animation *> animations;
    std::vector<CameraAnimation *> camera_animations;
    std::vector<GraphMathObject *> flat_scene_registry;

    GraphApp *app;
    GLFWwindow *window;
    Camera *camera;

    friend class GraphApp;
    friend class ICameraAnimatable;
    friend class ManualControl;
    friend class CameraControl;

protected:
    Scene(int width = 1200, int height = 600);
    virtual ~Scene();

    static void frame_size_buffer_callback(GLFWwindow *, int, int);
    void draw(float);

public:
    void InitGraphs();
    bool scene_exists() { return scene_instantiated; }
    Camera *getCamera() { return camera; }
    float getTime() const { return time_stamp; }
    
    void timeProgress(float t)
    {
        this->time_stamp += t;
        this->time_stamp = std::max(0.0f, this->time_stamp);
    }
    
    void run();

    void register_object(GraphMathObject *obj);
    void play(Animation *animation, float duration = 2.0f);
    void play(std::vector<Animation *> anims, float duration = 2.0f);
    void add(GraphMathObject *graph);
};

// --- Specialized Scene Templates ---

template <typename TControlPolicy>
class TwoDScene : public Scene, public TControlPolicy {
public:
    using Scene::play;

    TwoDScene(int width = 1200, int height = 600) : Scene(width, height) {
        camera->setPerspective(45.0f, (float)width/(float)height, 0.1f, 1000.0f);
    }
    virtual ~TwoDScene() = default;
};

template <typename TControlPolicy>
class ThreeDScene : public Scene, public TControlPolicy {
public:
    using Scene::play;

    ThreeDScene(int width = 1200, int height = 600) : Scene(width, height) {
        camera->setPerspective(45.0f, (float)width/(float)height, 0.1f, 1000.0f);
    }
    virtual ~ThreeDScene() = default;
};