#include "../../include/GraphEngine/Animations/Transition.hpp"

Transition::Transition(GraphMathObject *object, GraphMathObject *targetObj, float start_time, float duration) : Animation(object, targetObj, start_time, duration)
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
    targetObject->alignPoints(morphObject);

    int t_sub_obj_count = targetObject->subGraphObjects.size();
    int m_sub_obj_count = morphObject->subGraphObjects.size();

    int min_count = std::min(t_sub_obj_count, m_sub_obj_count);

    // Handle matching objects → Transition
    for (int i = 0; i < min_count; i++)
    {
        GraphMathObject *t_obj = targetObject->subGraphObjects[i];
        GraphMathObject *m_obj = morphObject->subGraphObjects[i];
        new Transition(t_obj, m_obj, start_time, duration);
    }

    // // Extra objects in target → FadeOut
    // if (t_sub_obj_count > m_sub_obj_count)
    // {
    //     for (int i = min_count; i < t_sub_obj_count; i++)
    //     {
    //         GraphMathObject *t_obj = targetObject->subGraphObjects[i];
    //         new FadeOut(t_obj, start_time, duration);
    //     }
    // }

    // // Extra objects in morph → ShowCreation
    // if (m_sub_obj_count > t_sub_obj_count)
    // {
    //     for (int i = min_count; i < m_sub_obj_count; i++)
    //     {
    //         GraphMathObject *m_obj = morphObject->subGraphObjects[i];
    //         new ShowCreation(m_obj, start_time, duration);
    //     }
    // }

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
        morphObject->showFill = false;
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
