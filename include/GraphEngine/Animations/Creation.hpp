#ifndef __CREATION_HPP__
#define __CREATION_HPP__

#include "Animation.hpp"

class ShowCreation: public Animation{

    public:
        glm::vec3 to;

    public: 
    ShowCreation(GraphObject* object, float start_time = 0, float duration = 2.0f): Animation(object, start_time, duration){
        targetObject->prepareSubObjects(start_time, duration);
    }
    void play(float dt) override;
};
class ShowFillCreation: public Animation{

    public:
        glm::vec3 to;

    public: 
    ShowFillCreation(GraphObject* object, float start_time = 0, float duration = 2.0f): Animation(object, start_time, duration){
        // targetObject->prepareSubObjects(start_time, duration);

    }
    void play(float dt) override;
};

class UnCreate: public Animation{

    public: 
    UnCreate(GraphObject* object, float start_time = 0, float duration = 2.0f): Animation(object, start_time, duration){}
    void play(float dt) override{
        if(dt < start_time) return;
        if(dt > end_time) {
            progress = 0;
            return;
        }
        float prg = (AnimationTimmingFunction::easeInOutExpo(1.0f - (dt - start_time), duration));

        targetObject->setProgress(prg < 0 ? 0 : prg);
    }
};

#endif