#include "../../include/GraphEngine/Animations/Transition.hpp"

Transition::Transition(GraphObject *object, GraphObject *targetObj, float start_time, float duration) : Animation(object, targetObj, start_time, duration)
{
}

void Transition::Init()
{
    originalBezierPoints = targetObject->getAllBezierPoints();
    std::vector<glm::vec3> targetBezierPoints = morphObject->getAllBezierPoints();

    // Match bezier arrays sizes by padding
    int b_res1 = originalBezierPoints.size();
    int b_res2 = targetBezierPoints.size();
    
    // We must ensure they are both cubic (multiples of 4, or at least consistent)
    // For simplicity, just pad with the last point.
    if (b_res1 > 0 && b_res2 > 0) {
        float epsilon = 0.0001f;
        while(originalBezierPoints.size() < targetBezierPoints.size()) {
             glm::vec3 last = originalBezierPoints.back();
             originalBezierPoints.push_back(last + glm::vec3(epsilon, 0, 0));
             epsilon += 0.0001f;
        }
        epsilon = 0.0001f;
        while(targetBezierPoints.size() < originalBezierPoints.size()) {
             glm::vec3 last = targetBezierPoints.back();
             targetBezierPoints.push_back(last + glm::vec3(epsilon, 0, 0));
             epsilon += 0.0001f;
        }
    }
    
    // Store the matched target set in the morphObject's flattened state for the duration
    this->targetBezierPointsMatched = targetBezierPoints;
    
    is_initialized = true;
}

void Transition::morphPoints(float alpha)
{
    std::vector<glm::vec3> current_pts;
    current_pts.reserve(originalBezierPoints.size());
    
    for (size_t i = 0; i < originalBezierPoints.size(); ++i) {
        glm::vec3 p = glm::mix(originalBezierPoints[i], targetBezierPointsMatched[i], alpha);
        current_pts.push_back(p);
    }
    
    targetObject->setAllBezierPoints(current_pts);
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

    float prog = anim_timing_func(dt - start_time, duration);

    morphPoints(prog);

}
