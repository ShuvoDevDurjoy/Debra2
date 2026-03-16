#ifndef __TRANSITION_HPP__
#define __TRANSITION_HPP__


#include "Animation.hpp"
#include "AnimationManager.hpp"
#include "Fade.hpp"
#include "Creation.hpp"


class Transition: public Animation{
    public :
        bool is_initialized = false;
        std::vector<glm::vec3> originalPoints;
        std::vector<glm::vec3> originalBezierPoints; // Store initial bezier curves state
        std::vector<glm::vec3> targetBezierPointsMatched;
        std::vector<glm::vec3> fillOriginalPoints;

    public:
        Transition(GraphMathObject *object, GraphMathObject *targetObj);
        void play(float dt) override;

        void morphPoints(float alpha = 0);
        void Init();
};

#endif