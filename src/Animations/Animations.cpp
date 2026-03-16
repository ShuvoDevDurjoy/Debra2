#include "../../include/GraphEngine/Animations/Animation.hpp"

Animation::Animation(GraphMathObject *object)
{
    targetObject = object;
    AnimationManager::addToManager(this);
}

Animation::Animation(GraphMathObject *object, GraphMathObject* morphObject)
{
    targetObject = object;
    this->morphObject = morphObject;
    AnimationManager::addToManager(this);
}

