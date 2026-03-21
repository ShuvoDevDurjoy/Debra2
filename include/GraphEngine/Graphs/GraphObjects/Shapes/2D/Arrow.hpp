#ifndef __ARROW_HPP__
#define __ARROW_HPP__

#include <GraphEngine/Graphs/GraphObjects/GraphObject.hpp>

// Arrow: a line with a filled arrowhead at the end.
// It is a GraphGroup composed of a Line body and a Triangle tip.
class Arrow : public GraphObject
{
public:
    Arrow(glm::vec3 start = glm::vec3(-5, 0, 0),
          glm::vec3 end = glm::vec3(5, 0, 0),
          float tip_size = 1.5f,
          float shaft_width = 0.6f,
          float buff = 0.1f)
        : source_start(start), source_end(end), tip_size(tip_size), shaft_width(shaft_width), buff(buff)
    {
        setStrokeWidth(0.5f);
        Init();
    }

    void generatePoints() override
    {
        float dist = glm::distance(source_start, source_end);
        if (dist < 0.0001f) return;

        glm::vec3 dir = glm::normalize(source_end - source_start);
        
        // Apply buffering like Manim
        glm::vec3 real_start = source_start + dir * buff;
        glm::vec3 real_end = source_end - dir * buff;

        if (glm::distance(real_start, real_end) < tip_size) {
            // Arrow too short for buffering, fallback or shorten
            real_start = source_start;
            real_end = source_end;
        }

        glm::vec3 perp = glm::vec3(-dir.y, dir.x, 0.0f);
        float h_half = tip_size * 0.6f;
        float s_half = shaft_width * 0.5f;

        // The point where shaft meets head
        glm::vec3 head_base = real_end - dir * tip_size;

        // Silhouette approach: define the entire filled outline
        start_bezier_path(real_start + perp * s_half); // 1. Tail Left
        add_line_to(head_base + perp * s_half);   // 2. Neck Left
        add_line_to(head_base + perp * h_half);   // 3. Head Wing Left
        add_line_to(real_end);                         // 4. Tip
        add_line_to(head_base - perp * h_half);   // 5. Head Wing Right
        add_line_to(head_base - perp * s_half);   // 6. Neck Right
        add_line_to(real_start - perp * s_half);       // 7. Tail Right
        add_line_to(real_start + perp * s_half);       // 8. Close loop

        float minx = std::min(real_start.x, real_end.x) - h_half;
        float maxx = std::max(real_start.x, real_end.x) + h_half;
        float miny = std::min(real_start.y, real_end.y) - h_half;
        float maxy = std::max(real_start.y, real_end.y) + h_half;
        setDimension(minx, maxx, miny, maxy);
    }

public:
    glm::vec3 source_start, source_end;
    float tip_size, shaft_width, buff;
};

#endif
