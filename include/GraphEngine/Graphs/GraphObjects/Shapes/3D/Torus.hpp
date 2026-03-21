#ifndef __TORUS_HPP__
#define __TORUS_HPP__

#include <GraphEngine/Graphs/GraphObjects/ThreeDObject.hpp>

class Torus : public ThreeDObject {
private:
    float major_radius;
    float minor_radius;
public:
    Torus(float major_radius = 2.0f, float minor_radius = 0.5f);
    void Init() override;
    
    static glm::vec3 torusFunc(float rho, float theta, Var var);
};

#endif
