#ifndef __TIMER_CPP__
#define __TIMER_CPP__

#include <cmath>

namespace Utils
{
    inline float round2(float n)
    {
        return std::round(n * 100) / 100;
    }
};

#endif