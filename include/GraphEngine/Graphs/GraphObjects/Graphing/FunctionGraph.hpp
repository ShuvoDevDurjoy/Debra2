#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <GraphEngine/Math/Var.hpp>
#include <GraphEngine/Graphs/GraphObjects/GraphObject.hpp>

class FunctionGraph : public GraphObject
{
public:
    glm::vec3 (*func)(float, Var);
    Var func_var;
    std::array<float, 3> range;

public:
    FunctionGraph(glm::vec3 (*f)(float, Var), std::array<float, 3> range = {-10.0f * M_PI, 10.0f * M_PI, 0.5f}, Var v = Var(0), bool make_smooth = true)
    {
        this->func = f;
        this->range = range;
        this->func_var = v;
        if (make_smooth)
            makeSmooth();
    }

    void generatePoints() override
    {
        if (!func)
            return;

        bezier_points.clear();
        bezier_sub_path_starts.clear();
        points.clear();
        point_sub_path_ranges.clear();

        float minx = 1e18, maxX = -1e18, minY = 1e18, maxY = -1e18, minZ = 1e18, maxZ = -1e18;
        int step_count = std::ceil((std::abs(range[1] - range[0])) / range[2]);
        float t = std::min(range[1], range[0]);

        for (int i = 0; i < step_count; ++i)
        {
            glm::vec3 p = func(t, func_var);

            if (i == 0)
            {
                start_bezier_path(p);
            }
            else
            {
                add_line_to(p);
            }

            t += range[2];

            minx = std::min(minx, p.x);
            maxX = std::max(maxX, p.x);
            minY = std::min(minY, p.y);
            maxY = std::max(maxY, p.y);
            minZ = std::min(minZ, p.z);
            maxZ = std::max(maxZ, p.z);
        }

        setDimension(minx, maxX, minY, maxY, minZ, maxZ);
    }
};