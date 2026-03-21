#ifndef __SPHERE_HPP__
#define __SPHERE_HPP__

#include <GraphEngine/Graphs/GraphObjects/ThreeDObject.hpp>

class Sphere : public ThreeDObject {
private:
    float radius;
public:
    Sphere(float radius = 1.0f);
    void Init() override;
    
    static glm::vec3 sphereFunc(float phi, float theta, Var var);
};

#endif
