#ifndef __CREATION_HPP__
#define __CREATION_HPP__

#include "Animation.hpp"

class ShowCreation: public Animation{

    public:
        glm::vec3 to;

    public: 
    ShowCreation(GraphMathObject* object): Animation(object){
        // targetObject->prepareSubObjects(start_time, duration);
    }
    void Init();
    void play(float dt) override;
};
class ShowFillCreation: public Animation{

    public:
        glm::vec3 to;

    public: 
    ShowFillCreation(GraphMathObject* object): Animation(object){
        // targetObject->prepareSubObjects(start_time, duration);

    }
    void play(float dt) override;
};

class UnCreate: public Animation{

    public: 
    UnCreate(GraphObject* object): Animation(object){}
    void play(float dt) override{
        if(dt < start_time) return;
        if(dt > end_time) {
            return;
        }
        float prg = (AnimationTimmingFunction::easeInOutExpo(1.0f - (dt - start_time), duration));

        targetObject->setStrokeProgress(prg < 0 ? 0 : prg);
    }
};

#endif