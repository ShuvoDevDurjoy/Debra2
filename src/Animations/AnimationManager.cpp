#include "../../include/GraphEngine/Animations/AnimationManager.hpp"

std::queue<Animation *> AnimationManager::animationObjectQueue;
std::unordered_set<Animation *> AnimationManager::animationObjectSet;

inline bool doesIntersect(float start_time_1, float end_time_1, float start_time_2, float end_time_2)
{
    return (start_time_1 <= end_time_2) && (start_time_2 < end_time_1);
}

void AnimationManager::addToManager(Animation *obj)
{

    auto it = std::find_if(animationObjectSet.begin(), animationObjectSet.end(), [obj](Animation *ob)
                           { return obj->targetObject == ob->targetObject; });
    if (it == animationObjectSet.end())
    {
        animationObjectSet.insert(obj);
        animationObjectQueue.push(obj);
    }
    else
    {
        Animation *anim_obj = (*it);
        if (!doesIntersect(anim_obj->start_time, anim_obj->end_time, obj->start_time, obj->end_time))
        {
            animationObjectSet.insert(obj);
            animationObjectQueue.push(obj);
        }
    }
}

void AnimationManager::Run(float dt)
{
    int size = animationObjectQueue.size();
    for (int i = 0; i < size; ++i)
    {
        Animation *anim = animationObjectQueue.front();
        if(anim->start_time <= dt){
            anim->play(dt);
            anim->targetObject->updatePoints();
        }
        if(anim->end_time >= dt){
            animationObjectQueue.push(anim);
        }
        animationObjectQueue.pop();
    }
}
