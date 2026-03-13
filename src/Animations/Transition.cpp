#include "../../include/GraphEngine/Animations/Transition.hpp"

Transition::Transition(GraphObject *object, GraphObject *targetObj, float start_time, float duration) : Animation(object, targetObj, start_time, duration)
{
}

inline void showPoints(std::vector<glm::vec3> pts)
{
    for (auto p : pts)
    {
        std::cout << "(" << p.x << "," << p.y << "," << p.z << ") , ";
    }
    std::cout << std::endl;
}

inline void showRange(std::vector<std::pair<int,int>> ranges)
{
    for (auto r : ranges)
    {
        std::cout << "(" << r.first << "," << r.second << ") , ";
    }
    std::cout << std::endl;
}

void Transition::Init()
{
    std::cout << "Before starting Animation" << std::endl;
    // showPoints(targetObject->points);
    // showPoints(morphObject->points);
    // showRange(targetObject->point_sub_path_ranges);
    // showRange(morphObject->point_sub_path_ranges);
    std::cout << targetObject->getPointsSize() << std::endl;
    std::cout << morphObject->getPointsSize() << std::endl;
    targetObject->alignPoints(morphObject);
    std::cout << "After aligning points" << std::endl;
    std::cout << targetObject->getPointsSize() << std::endl;
    std::cout << morphObject->getPointsSize() << std::endl;
    // showPoints(targetObject->points);
    // showPoints(morphObject->points);
    // showRange(targetObject->point_sub_path_ranges);
    // showRange(morphObject->point_sub_path_ranges);
    is_initialized = true;
}

void Transition::morphPoints(float alpha)
{
    if(!is_initialized)
        return;
    targetObject->interpolate(morphObject, alpha);
}

void Transition::play(float dt)
{
    if (dt < start_time)
    return;
    if (!is_initialized)
    {
        morphObject->showGraph = false;
        std::cout << "Initializing Transition Animation" << std::endl;
        Init();
    }

    if (dt >= end_time) {
        morphPoints(1.0f);
        return;
    }
    float prog = anim_timing_func(dt - start_time, duration);
    morphPoints(prog);

}
