#include "../../include/GraphEngine/Animations/Creation.hpp"

void ShowCreation::Init()
{
    targetObject->showGraph = false;
    if (!targetObject->subGraphObjects.empty())
    {
        //how many sub object is in
        float buffer = 0.5f;
        int count = targetObject->subGraphObjects.size();
        float segment_duration = duration / count;
        float ds = (1.0f - buffer) * segment_duration;
        float dt = std::max(buffer * duration, 0.1f);
        int index = 0;
        this->targetObject->showGraph = false;

        for (GraphMathObject *sub : targetObject->subGraphObjects)
        {
            sub->showGraph = false;
            (new ShowCreation(sub, start_time + index * ds, dt))->anim_timing_func = this->anim_timing_func;
            index++;
        }
    }

    is_initialized = true;
}

void ShowCreation::play(float dt)
{
    if (dt < start_time)
    {
        progress = 0;
        return;
    }
    if(!is_initialized)
    Init();
    if (dt >= end_time)
    {
        progress = 1;
        targetObject->setStrokeProgress(1);
        targetObject->setFillProgress(1);
        return;
    }

    
    float prg = (anim_timing_func((dt - start_time), duration));
    targetObject->showGraph = true;
    targetObject->setStrokeProgress(prg < 0 ? 0 : prg);
    targetObject->setFillProgress(prg < 0 ? 0 : prg);
}

void ShowFillCreation::play(float dt)
{
    if (dt < start_time)
    {
        fillProgress = 0;
        targetObject->setFillProgress(0);
        return;
    }
    if (dt > end_time)
    {
        fillProgress = 1;
        targetObject->setFillProgress(1);
        return;
    }

    float prg = (anim_timing_func((dt - start_time), duration));

    targetObject->setFillProgress(prg < 0 ? 0 : prg);
}