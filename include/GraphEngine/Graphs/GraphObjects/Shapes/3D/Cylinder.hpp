#ifndef __CYLINDER_HPP__
#define __CYLINDER_HPP__

#include <GraphEngine/Graphs/GraphObjects/ThreeDObject.hpp>

class Cylinder : public ThreeDObject {
private:
    float radius;
    float height;
public:
    Cylinder(float radius = 1.0f, float height = 2.0f);
    void Init() override;
    
    static glm::vec3 cylinderFunc(float rho, float theta, Var var);
};

#endif
