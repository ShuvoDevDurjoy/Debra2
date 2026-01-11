#ifndef __ANIMATION_HPP__
#define __ANIMATION_HPP__

#include "../Graphs/GraphObjects/GraphObject.hpp"
#include <queue>

#include "AnimationManager.hpp"
#include "../Utils/AnimationMode.hpp"
#include "../Utils/Timer.hpp"

class GraphObject;

class Animation
{
public:
    glm::vec3 translation_factor;
    glm::vec3 rotation_factor;
    int drawStart;
    int drawSize;
    float progress = 1.0f;
    float fillProgress = 1.0f;
    float start_time, duration, end_time;
    glm::vec3 pos;
    bool updatingPos = false;
    GraphObject *targetObject;
    GraphObject *morphObject;
    bool is_initialized = false;

public:
    Animation(GraphObject *object, float start_time = 0.0f, float duration = 2.0f);
    Animation(GraphObject *object, GraphObject* morphObject, float start_time = 0.0f, float duration = 2.0f);
    void updateState(float dt);
    void updateProgress(float dt);
    void updateFillProgress(float dt);
    void updatePosition();
    virtual void play(float dt) = 0;
};

#endif