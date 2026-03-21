#ifndef __CURVEDARROW_HPP__
#define __CURVEDARROW_HPP__

#include <GraphEngine/Graphs/GraphObjects/GraphObject.hpp>

class CurvedArrow : public GraphObject {
public:
    glm::vec3 start_anchor, control1, control2, end_anchor;
    float tip_size, shaft_width, buff;
    bool is_cubic;

public:
    // Quadratic constructor
    CurvedArrow(glm::vec3 start, glm::vec3 control, glm::vec3 end,
                float tip_size = 1.5f, float shaft_width = 0.6f, float buff = 0.1f);

    // Cubic constructor
    CurvedArrow(glm::vec3 start, glm::vec3 control1, glm::vec3 control2, glm::vec3 end,
                float tip_size = 1.5f, float shaft_width = 0.6f, float buff = 0.1f);

    void generatePoints() override;
};

#endif
