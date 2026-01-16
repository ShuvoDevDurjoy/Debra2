#include "../../include/GraphEngine/Animations/Animation.hpp"

Animation::Animation(GraphMathObject *object, float start_time, float duration)
{
    targetObject = object;
    this->start_time = start_time;
    this->duration = duration;
    this->end_time = this->start_time + duration;
    drawStart = 0;
    drawSize = targetObject->getPointsSize();
    progress = 1.0f;
    std::cout << "Start time is: " << start_time << std::endl;
    AnimationManager::addToManager(this);
}
Animation::Animation(GraphMathObject *object, GraphMathObject* morphObject, float start_time, float duration)
{
    targetObject = object;
    this->morphObject = morphObject;
    this->start_time = start_time;
    this->duration = duration;
    this->end_time = start_time + duration;
    drawStart = 0;
    drawSize = targetObject->getPointsSize();
    progress = 1.0f;
    AnimationManager::addToManager(this);
}

void Animation::updatePosition(){
    std::cout << "Current Position is: " << pos.x << " and " << pos.y << std::endl;
    targetObject->moveTo(pos);
}

void Animation::updateProgress(float dt){
    
}

void Animation::updateFillProgress(float dt){
}

void Animation::updateState(float dt)
{
    // targetObject->setDrawStart(drawStart);
    // targetObject->setDrawSize(drawSize);
    updateProgress(dt);
    updateFillProgress(dt);
    if(updatingPos){
        updatePosition();
    }
}

