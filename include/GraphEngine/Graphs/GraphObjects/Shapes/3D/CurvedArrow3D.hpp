#ifndef __CURVEDARROW3D_HPP__
#define __CURVEDARROW3D_HPP__

#include <GraphEngine/Graphs/GraphObjects/ThreeDObject.hpp>

class CurvedArrow3D : public ThreeDObject {
public:
    glm::vec3 start_anchor, control1, control2, end_anchor;
    float shaft_radius, head_length;
    bool is_cubic;

public:
    CurvedArrow3D(glm::vec3 start, glm::vec3 control, glm::vec3 end,
                  float shaft_radius = 0.5f, float head_length = 2.0f);

    CurvedArrow3D(glm::vec3 start, glm::vec3 control1, glm::vec3 control2, glm::vec3 end,
                  float shaft_radius = 0.5f, float head_length = 2.0f);

    void generatePoints() override;
};

#endif
