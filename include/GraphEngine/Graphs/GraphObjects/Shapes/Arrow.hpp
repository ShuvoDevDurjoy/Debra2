#ifndef __ARROW_HPP__
#define __ARROW_HPP__

#include "../GraphObject.hpp"
#include "../GraphGroup.hpp"

// Arrow: a line with a filled arrowhead at the end.
// It is a GraphGroup composed of a Line body and a Triangle tip.
class Arrow : public GraphObject
{
public:
    float tip_size;
    float shaft_width;

    // start: tail of the arrow, end: tip of the arrow
    Arrow(glm::vec3 start = glm::vec3(-5, 0, 0),
          glm::vec3 end = glm::vec3(5, 0, 0),
          float tip_size = 1.5f,
          float shaft_width = 0.6f)
        : tip_size(tip_size), shaft_width(shaft_width)
    {
        glm::vec3 dir = glm::normalize(end - start);
        glm::vec3 perp = glm::vec3(-dir.y, dir.x, 0.0f);
        float h_half = tip_size * 0.6f;
        float s_half = shaft_width * 0.5f;

        // The point where shaft meets head
        glm::vec3 head_base = end - dir * tip_size;

        // Silhouette approach: define the entire filled outline
        start_bezier_path(start + perp * s_half); // 1. Tail Left
        add_line_to(head_base + perp * s_half);   // 2. Neck Left
        add_line_to(head_base + perp * h_half);   // 3. Head Wing Left
        add_line_to(end);                         // 4. Tip
        add_line_to(head_base - perp * h_half);   // 5. Head Wing Right
        add_line_to(head_base - perp * s_half);   // 6. Neck Right
        add_line_to(start - perp * s_half);       // 7. Tail Right
        add_line_to(start + perp * s_half);       // 8. Close loop

        float minx = std::min(start.x, end.x) - h_half;
        float maxx = std::max(start.x, end.x) + h_half;
        float miny = std::min(start.y, end.y) - h_half;
        float maxy = std::max(start.y, end.y) + h_half;
        setDimension(minx, maxx, miny, maxy);
        build_points_from_bezier();

        // Default to showing fill for a "solid" arrow look
        fillOpacity = 1.0f;
    }
};

#endif
