#ifndef __STAR_HPP__
#define __STAR_HPP__

#include <GraphEngine/Graphs/GraphObjects/GraphObject.hpp>

class Star : public GraphObject
{
public:
    Star(float outer_radius, float inner_radius, int points = 5, float x = 0, float y = 0)
    {
        float angle_step = glm::pi<float>() / points;

        std::vector<glm::vec3> anchors;
        for (int i = 0; i < 2 * points; ++i)
        {
            float r = (i % 2 == 0) ? outer_radius : inner_radius;
            float angle = i * angle_step - glm::half_pi<float>();
            anchors.push_back(glm::vec3(x + r * cos(angle), y + r * sin(angle), 0));
        }

        start_bezier_path(anchors[0]);
        for (size_t i = 1; i < anchors.size(); ++i)
        {
            add_line_to(anchors[i]);
        }
        add_line_to(anchors[0]);

        setDimension(x - outer_radius, x + outer_radius, y - outer_radius, y + outer_radius);
    }
};

#endif
