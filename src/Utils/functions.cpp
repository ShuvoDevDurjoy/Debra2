#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__
#include <cmath>
#include "../../include/GraphEngine/Graphs/Graph.hpp"

class Functions
{
public:
    static float sineModulated(float x)
    {
        return 15.0f * GraphUtilities::genSin(x * 5.0f) * exp(-1.0f * pow(x, 2.0f));
        // return GraphUtilities::genSin(x);
    }
    static float tangentInversion(float x)
    {
        return GraphUtilities::genTan(x) * GraphUtilities::genSin(x);
    }
    static float butterflyPolar(float x)
    {
        return (exp(GraphUtilities::genSin(x)) - 2.0f * GraphUtilities::genCos(4.0f * x) + pow(GraphUtilities::genSin((20.0f - M_PI) / 24.0f), 5)) * 1.5f;
    }
    static float cubicQuardratic(float x)
    {
        return pow(x, 3.0f) - 3.0f * x;
    }
    static float roseCurve(float x)
    {
        return cos(8.0f * x);
    }
    static float cardroid(float x)
    {
        return 1 - sin(x);
    }
    static float spiralArchimedes(float x)
    {
        return 1.0f * x;
    }
    static float logarithmicSpiral(float x)
    {
        return 1.0f * exp(1.0f * x);
    }
    static float fermatSpiral(float x)
    {
        return sqrt(x);
    }
    static float mandelbrotEsque(float x)
    {
        return GraphUtilities::genSin(pow(x, 2.0f) + GraphUtilities::genCos(3.0f * x) * GraphUtilities::genCos(x));
    }
    static float mandelbrotEsqueRadians(float x)
    {
        return sin(pow(x, 2.0f) + cos(3.0f * x) * cos(x));
    }
    static float superFlower(float x)
    {
        return sin(7.0f * x) + cos(3.0f * x);
    }

    static std::pair<float, float> spirograph(float t, Var v)
    {
        float R = v[1], r = v[2], d = 3.0f;
        float k = (R + r) / r;
        float x = (R + r) * cos(t) - d * cos(k * t);
        float y = (R + r) * sin(t) - d * sin(k * t);
        return {x * 0.5f + v[0], y * 0.5f}; // scale down
    }

    static std::pair<float, float> superformula(float theta)
    {
        float m = 7, a = 1, b = 1;
        float n1 = 0.3f, n2 = 0.3f, n3 = 0.3f;

        float part1 = pow(fabs(cos(m * theta / 4) / a), n2);
        float part2 = pow(fabs(sin(m * theta / 4) / b), n3);
        float r = pow(part1 + part2, -1.0f / n1);

        float x = r * cos(theta);
        float y = r * sin(theta);
        return {x * 0.5f, y * 0.5f};
    }

    static std::pair<float, float> maurerRose(float t, Var v)
    {
        float k = v[0]; // petal factor
        float r = v[1] * sin(k * t);
        float x = r * cos(t) + v[2];
        float y = r * sin(t);
        return {x, y};
    }

    static std::pair<float, float> waveSpiral(float t)
    {
        float x = t * cos(t) + sin(3 * t);
        float y = t * sin(t) + cos(3 * t);
        return {x * 0.05f, y * 0.05f};
    }

    static std::pair<float, float> harmonograph(float t)
    {
        float a = 2.0f, b = 3.0f, d = 0.005f, delta = M_PI / 2;
        float x = sin(a * t + delta) * exp(-d * t);
        float y = sin(b * t) * exp(-d * t);
        return {x, y};
    }

    static std::pair<float, float> heartCurve(float t, Var v)
    {
        float x = v[1] + 16 * pow(sin(t), 3);
        float y = v[2] + 13 * cos(t) - 5 * cos(2 * t) - 2 * cos(3 * t) - cos(4 * t);
        return {x * v[0], y * v[0]};
    }

    static std::pair<float, float> lissajous(float t, Var v)
    {
        float A = 5.0f, B = 5.0f, a = 3.0f, b = 2.0f, delta = M_PI / 2;
        float x = A * sin(a * t + delta) + v[0];
        float y = B * sin(b * t);
        return {x, y};
    }

    static std::pair<float, float> hypotrochoid(float t)
    {
        float R = 5.0f, r = 3.0f, d = 5.0f;
        float k = (R - r) / r;
        float x = (R - r) * cos(t) + d * cos(k * t);
        float y = (R - r) * sin(t) - d * sin(k * t);
        return {x, y};
    }

    static float genLine(float x)
    {
        float m = tan(GraphUtilities::toRadians(45.0f));
        float c = 2.0f;
        float y = m * x + c;
        return y;
    }

    static std::pair<float, float> riemannProjection(float t)
    {
        // Assume t goes from -π to π
        float r = tanf(t);
        float theta = 10.0f * t;

        float x = r * cos(theta);
        float y = r * sin(theta);

        // This spirals outward super fast; normalize it
        float scale = 1.0f / (1.0f + fabs(r));

        return {x * scale, y * scale};
    }

    static float explodedTrigFusion(float x, Var v)
    {
        return v[0] * (sin(x * sin(3 * x)) + cos(x * cos(5 * x))) * exp(cos(x)) * tan(sin(x * 2.0f)) * v[1];
    }
    static float explodedTrigFusion2(float x)
    {
        return 10.0f * (GraphUtilities::genSin(x * GraphUtilities::genSin(3 * x)) + GraphUtilities::genCos(x * GraphUtilities::genCos(5 * x))) * exp(GraphUtilities::genCos(x)) * tan(GraphUtilities::genSin(x * 2.0f)) * 0.05f;
    }
    static float explodedTrigFusion3(float x)
    {
        return 10.0f * (GraphUtilities::genSin(x * GraphUtilities::genSin(3 * x)) + GraphUtilities::genCos(x * GraphUtilities::genCos(5 * x))) * exp(GraphUtilities::genCos(x)) * tan(GraphUtilities::genSin(x * 2.0f)) * 0.05f;
    }

    static std::pair<float, float> chaoticFractal(float t, Var v)
    {
        float x = sin(t) * cos(3 * t) + cos(t * t);
        float y = cos(t) * sin(5 * t) - sin(t * t);
        return {x * v[0] + v[1], y * v[0] + v[2]};
    }

    static std::pair<float, float> spikeSpiral(float t)
    {
        float r = exp(sin(6 * t)) + tan(2 * t); // Boom
        return {r * cos(t) * 0.1f, r * sin(t) * 0.1f};
    }

    static float something(float x)
    {
        return GraphUtilities::genTan(x);
    }
    static float something2(float x)
    {
        return tan(x);
    }

    static float graphButterfly(float x, Var v)
    {
        float y = (exp(sin(x)) - 2.0f * cos(4.0f * x) + pow(sin(x / 12.0f), 5.0f)) * v[0];
        return y;
    }

    static std::pair<float, float> graphButterflyParam(float t, Var v)
    {
        // scale factor: v[0]
        float k = v[0];

        float common =
            exp(sin(t)) -
            2.0f * cos(4.0f * t) +
            pow(sin(t / 12.0f), 5.0f);

        float x = k * common * sin(t);
        float y = k * common * cos(t);

        float xr = -y + v[1];
        float yr = x + v[2];

        return {xr, yr};
    }

    static std::pair<float, float> para3Param(float t, Var v)
    {
        float x = t;

        float y = v[2] +
                  v[1] * pow(cos((1.0f - (1.0f / v[0])) * t), 2.0f);

        return {x, y};
    }

    static std::pair<float, float> hypotrocloid(float t)
    {
        float R = 5.0f;
        float r = 3.0f;
        float d = 1.5f; // try changing this to sharpen or flatten the star tips

        float k = (R - r) / r;
        float x = (R - r) * cos(t) + d * cos(k * t);
        float y = (R - r) * sin(t) - d * sin(k * t); // minus here gives standard orientation
        return {x, y};
    }

    static float para3(float x, Var v)
    {
        // return ( x / 4.0f) +  10.0f * pow(cos((1.0f - (1.0f / a)) * a * x), 2.0f);
        return v[2] + v[1] * pow(cos((1.0f - (1.0f / v[0])) * x), 2.0f);
    }
};
#endif // __FUNCTIONS_H__
