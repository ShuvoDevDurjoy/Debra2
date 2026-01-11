#pragma once
#ifndef __ANIMATIONMODE__HPP__
#define __ANIMATIONMODE__HPP__

enum AnimationMode
{
    ONCE = 0,
    INFINITE = 1,
    ONCE_AND_REMOVE = 2,
    ONCE_AND_LOOP_BACK = 3
};

namespace AnimationTimmingFunction
{
    inline float linearProgress(float elapsedTime, float duration)
    {
        float t = elapsedTime / duration;
        return t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
    }
    inline float easeInQuad(float elapsedTime, float duration)
    {
        float t = elapsedTime / duration;
        t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
        return t * t;
    }

    inline float easeOutQuad(float elapsedTime, float duration)
    {
        float t = elapsedTime / duration;
        t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
        return t * (2.0f - t);
    }

    inline float easeInOutQuad(float elapsedTime, float duration)
    {
        float t = elapsedTime / duration;
        t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
        return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
    }

    inline float easeInCubic(float elapsedTime, float duration)
    {
        float t = elapsedTime / duration;
        t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
        return t * t * t;
    }

    inline float easeOutCubic(float elapsedTime, float duration)
    {
        float t = elapsedTime / duration - 1.0f;
        t = t < -1.0f ? -1.0f : (t > 0.0f ? 0.0f : t);
        return t * t * t + 1.0f;
    }

    inline float easeInOutCubic(float elapsedTime, float duration)
    {
        float t = elapsedTime / duration;
        t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
        return t < 0.5f ? 4.0f * t * t * t : 1.0f - pow(-2.0f * t + 2.0f, 3) / 2.0f;
    }

    inline float easeInSine(float elapsedTime, float duration)
    {
        float t = elapsedTime / duration;
        t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
        return 1.0f - cos((t * 3.14159265f) / 2.0f);
    }

    inline float easeOutSine(float elapsedTime, float duration)
    {
        float t = elapsedTime / duration;
        t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
        return sin((t * 3.14159265f) / 2.0f);
    }

    inline float easeInOutSine(float elapsedTime, float duration)
    {
        float t = elapsedTime / duration;
        t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
        return -(cos(3.14159265f * t) - 1.0f) / 2.0f;
    }

    inline float easeInExpo(float elapsedTime, float duration)
    {
        float t = elapsedTime / duration;
        t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
        return (t == 0.0f) ? 0.0f : pow(2.0f, 10.0f * (t - 1.0f));
    }

    inline float easeOutExpo(float elapsedTime, float duration)
    {
        float t = elapsedTime / duration;
        t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
        return (t == 1.0f) ? 1.0f : 1.0f - pow(2.0f, -10.0f * t);
    }

    inline float easeInOutExpo(float elapsedTime, float duration)
    {
        float t = elapsedTime / duration;
        t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
        if (t == 0.0f)
            return 0.0f;
        if (t == 1.0f)
            return 1.0f;
        return t < 0.5f ? pow(2.0f, 20.0f * t - 10.0f) / 2.0f : (2.0f - pow(2.0f, -20.0f * t + 10.0f)) / 2.0f;
    }
}

#endif