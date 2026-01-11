#ifndef __ANIMATION_MANAGER_HPP__
#define __ANIMATION_MANAGER_HPP__

#include "../Graphs/GraphObjects/GraphObject.hpp"
#include "Animation.hpp"
#include <vector>
#include <unordered_set>
#include <queue>
class GraphObject;
class Animation;

struct AnimationObject{
    Animation *animation;
    float start_time;
    float end_time;

    AnimationObject(Animation* animation, float start_time, float end_time){
        this->animation = animation;
        this->start_time = start_time;
        this->end_time = end_time;
    }
};

class AnimationManager
{
private:
    static std::queue<Animation*> animationObjectQueue;
    static std::unordered_set<Animation*> animationObjectSet;

public:
    static void addToManager(Animation *obj);

    static void Run(float);
};

#endif