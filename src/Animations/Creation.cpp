#include "../../include/GraphEngine/Animations/Creation.hpp"

void ShowCreation::play(float dt)
{
    if (dt < start_time)
    {
        progress = 0;
        return;
    }
    if (dt >= end_time)
    {
        progress = 1;
        targetObject->setProgress(1);
        targetObject->setFillProgress(1);
        return;
    }

    float prg = (AnimationTimmingFunction::linearProgress((dt - start_time), duration));

    targetObject->setProgress(prg < 0 ? 0 : prg);
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

    float prg = (AnimationTimmingFunction::easeInOutExpo((dt - start_time), duration));

    targetObject->setFillProgress(prg < 0 ? 0 : prg);
}