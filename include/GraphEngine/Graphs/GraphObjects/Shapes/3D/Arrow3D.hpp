#ifndef __ARROW3D_HPP__
#define __ARROW3D_HPP__

#include <GraphEngine/Graphs/GraphObjects/ThreeDObject.hpp>

class Arrow3D : public ThreeDObject {
private:
    float length;
    float shaft_radius, head_length, buff;
    glm::vec3 from, to;

public:
    Arrow3D(glm::vec3 from = glm::vec3(0, 0, 0), glm::vec3 to = glm::vec3(0, 0, 20), float shaft_radius = 0.4f, float head_length = 1.5f, float buff = 0.1f);
    void generatePoints() override;
    void setFillData() override;
    void setStrokeData() override;
};

#endif
