#ifndef __ANIMATION_HPP__
#define __ANIMATION_HPP__

#include <queue>

#include <GraphEngine/Utils/Timer.hpp>
#include <GraphEngine/Utils/AnimationMode.hpp>
#include <GraphEngine/Animations/AnimationManager.hpp>
#include <GraphEngine/Graphs/GraphObjects/GraphObject.hpp>
#include <GraphEngine/Graphs/GraphObjects/GraphMathObject.hpp>

class GraphObject;
class GraphMathObject;

class Animation
{
    friend class Graph;

private:
    int priority = 0;

public:
    float start_time, duration, end_time;
    GraphMathObject *targetObject;
    GraphMathObject *morphObject;
    bool is_initialized = false;
    float (*anim_timing_func)(float, float) = AnimationTimmingFunction::easeInOutExpo;

public:
    Animation(GraphMathObject *object);
    Animation(GraphMathObject *object, GraphMathObject *morphObject);
    virtual void play(float dt) = 0;
};

#endif