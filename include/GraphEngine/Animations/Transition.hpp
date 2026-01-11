#ifndef __TRANSITION_HPP__
#define __TRANSITION_HPP__


#include "Animation.hpp"
#include "AnimationManager.hpp"


class Transition: public Animation{
    public :
        bool is_initialized = false;
        std::vector<glm::vec3> originalPoints;
        std::vector<glm::vec3> fillOriginalPoints;

    public:
        Transition(GraphObject *object, GraphObject *targetObj, float start_time = 0.0f, float duration = 3.0f);
        void play(float dt) override;

        void morphPoints(float alpha = 0);
        void Init();
};

#endif