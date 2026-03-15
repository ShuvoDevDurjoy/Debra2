#ifndef __NUMBER_LINE_HPP__
#define __NUMBER_LINE_HPP__

#include <GraphEngine/Graphs/GraphObjects/GraphObject.hpp>
#include <GraphEngine/Graphs/GraphObjects/Shapes/Arrow.hpp>
#include <GraphEngine/Graphs/GraphObjects/Shapes/Text.hpp>
#include <cmath>
#include <string>
#include <iomanip>
#include <sstream>

// NumberLine: a horizontal axis with evenly-spaced tick marks and numeric labels.
// Similar to Manim's NumberLine.
class NumberLine : public GraphObject
{
public:
    float x_min, x_max;
    float tick_step;
    float tick_height;
    float y_pos;
    float unit_size = 1.0f;

    NumberLine(float x_min = -10.0f,
               float x_max = 10.0f,
               float tick_step = 1.0f,
               float tick_height = 0.4f,
               float y_pos = 0.0f,
               float unit_size = 1.0f)
        : x_min(x_min), x_max(x_max),
          tick_step(tick_step), tick_height(tick_height), y_pos(y_pos), unit_size(unit_size)
    {
        float world_min = x_min * unit_size;
        float world_max = x_max * unit_size;

        // Main axis line
        start_bezier_path(glm::vec3(world_min, y_pos, 0));
        add_line_to(glm::vec3(world_max, y_pos, 0));
        setDimension(world_min, world_max, y_pos, y_pos);

        // Tick marks
        float v = x_min;
        while (v <= x_max + 1e-5f)
        {
            float wx = v * unit_size;
            start_bezier_path(glm::vec3(wx, y_pos - tick_height * 0.5f, 0));
            add_line_to(glm::vec3(wx, y_pos + tick_height * 0.5f, 0));
            v += tick_step;
        }

        setDimension(world_min, world_max, y_pos - tick_height, y_pos + tick_height);
        build_points_from_bezier();
    }

    void add_label(float label_size = 5.0f, float buff = 0.0f, int precision = 2)
    {
        float v = x_min;

        while (v <= x_max + 1e-5f)
        {
            float wx = v * unit_size;

            // Round and convert to string
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(precision) << v;
            std::string label = ss.str();
            
            // Remove trailing zeros
            if (label.find('.') != std::string::npos)
            {
                label.erase(label.find_last_not_of('0') + 1, std::string::npos);
                if (label.back() == '.')
                label.pop_back();
            }
            
            Text *text = new Text(label, wx, y_pos + buff, label_size);

            add(text);

            // BasePolygon *poly = new BasePolygon(3, 5, wx, y_pos + buff);
            // add(poly);

            v += tick_step;
        }
    }

    // Return the world-x position for a given data value
    float point_to_world(float val, float unit_size = 1.0f) const
    {
        return val * unit_size;
    }
};

#endif
