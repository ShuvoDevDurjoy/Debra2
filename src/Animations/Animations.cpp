#include "../../include/GraphEngine/Animations/Animation.hpp"

Animation::Animation(GraphObject *object, float start_time, float duration)
{
    targetObject = object;
    this->start_time = start_time;
    this->duration = duration;
    this->end_time = this->start_time + duration;
    drawStart = 0;
    drawSize = targetObject->getSize();
    progress = 1.0f;
    std::cout << "Start time is: " << start_time << std::endl;
    AnimationManager::addToManager(this);
}
Animation::Animation(GraphObject *object, GraphObject* morphObject, float start_time, float duration)
{
    targetObject = object;
    this->morphObject = morphObject;
    this->start_time = start_time;
    this->duration = duration;
    this->end_time = start_time + duration;
    drawStart = 0;
    drawSize = targetObject->getSize();
    progress = 1.0f;
    AnimationManager::addToManager(this);
}

void Animation::updatePosition(){
    std::cout << "Current Position is: " << pos.x << " and " << pos.y << std::endl;
    targetObject->moveTo(pos);
}

void Animation::updateProgress(float dt){
    std::queue<GraphObject *> objQueue;
    int maxDepth = 10;
    int cur_depth = 0;
    objQueue.push(targetObject);
    while(!objQueue.empty()){
        int size = objQueue.size();
        cur_depth++;
        if(cur_depth > maxDepth)
            break;
        for (int i = 0; i < size; ++i)
        {
            GraphObject *currObj = objQueue.front();
            objQueue.pop();
            float prg = (AnimationTimmingFunction::easeInOutExpo((dt - currObj->startTime), currObj->duration));
            prg = prg < 0.0f ? 0.0f : prg;
            prg = prg > 1.0f ? 1.0f : prg;
            currObj->setProgress(prg < 0 ? 0 : prg);
            for (GraphObject *subObj : currObj->subGraphObjects)
            {
                objQueue.push(subObj);
            }
        }
    }
}

void Animation::updateFillProgress(float dt){
    fillProgress = Utils::round2(AnimationTimmingFunction::easeInOutExpo(dt - start_time, duration));
    targetObject->setFillProgress(fillProgress);
}

void Animation::updateState(float dt)
{
    targetObject->setDrawStart(drawStart);
    targetObject->setDrawSize(drawSize);
    updateProgress(dt);
    updateFillProgress(dt);
    if(updatingPos){
        updatePosition();
    }
}

