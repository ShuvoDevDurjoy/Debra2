#include <GraphEngine/Graphs/GraphObjects/Shapes/3D/Sphere.hpp>

glm::vec3 Sphere::sphereFunc(float phi, float theta, Var var) {
    float r = var[0];
    float x = r * sin(phi) * cos(theta);
    float z = r * sin(phi) * sin(theta);
    float y = r * cos(phi);
    return glm::vec3(x, y, z);
}

Sphere::Sphere(float r) : radius(r) {
    this->rho_range = {0.0f, M_PI};
    this->theta_range = {0.0f, 2.0f * M_PI};
    this->resolution = {32, 32};
    this->graph_func = &Sphere::sphereFunc;
    
    this->graph_var = Var(r);
}

void Sphere::Init() {
    ThreeDObject::Init();
}
