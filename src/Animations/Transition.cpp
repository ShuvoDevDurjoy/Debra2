#include "../../include/GraphEngine/Animations/Transition.hpp"

Transition::Transition(GraphObject *object, GraphObject *targetObj, float start_time, float duration) : Animation(object, targetObj, start_time, duration)
{
}

void Transition::Init()
{
    
    int resolution1 = targetObject->getSize() - 1;
    int resolution2 = morphObject->getSize() - 1;
    if (resolution1 > resolution2)
    {
        morphObject->interpolate(resolution1);
        targetObject->interpolate(resolution1);
    }
    else
    {
        targetObject->interpolate(resolution2);
        morphObject->interpolate(resolution2);
    }
    
    originalPoints = targetObject->points;
    fillOriginalPoints = targetObject->current_fill_points;
    
    is_initialized = true;
}

void Transition::morphPoints(float alpha)
{
    for (int i = 0; i < targetObject->getSize(); ++i)
    {
        glm::vec3 p = glm::mix(originalPoints[i], morphObject->points[i], alpha);
        targetObject->points[i] = p;
    }
    
    for (int i = 0; i < targetObject->getFillSize(); ++i){
        glm::vec3 p = glm::mix(fillOriginalPoints[i], morphObject->current_fill_points[i], alpha);
        targetObject->current_fill_points[i] = p;
    }

}

void Transition::play(float dt)
{
    if (dt < start_time)
        return;
    else if(dt >= end_time){
        morphPoints(1);
    }
    else if(dt > end_time){
        return;
    }

    if (!is_initialized)
    {
        Init();
    }

    float prog = AnimationTimmingFunction::easeInOutExpo(dt - start_time, duration);

    morphPoints(prog);

}
