#ifndef __LINE3D_HPP__
#define __LINE3D_HPP__

#include <GraphEngine/Graphs/GraphObjects/ThreeDObject.hpp>

class Line3D : public ThreeDObject {
public:
    glm::vec3 from, to;
    float thickness;

public:
    Line3D(glm::vec3 from = glm::vec3(-10, 0, 0), glm::vec3 to = glm::vec3(10, 0, 0), float thickness = 0.5f);

    void generatePoints() override;
};

#endif
