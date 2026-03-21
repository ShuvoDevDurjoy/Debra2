#include <GraphEngine/Graphs/GraphObjects/Shapes/3D/Torus.hpp>

glm::vec3 Torus::torusFunc(float rho, float theta, Var var) {
    float R = var[0];
    float r = var[1];
    
    // Here we map rho to 'v' and theta to 'u'
    float x = (R + r * cos(rho)) * cos(theta);
    float z = (R + r * cos(rho)) * sin(theta);
    float y = r * sin(rho);
    
    return glm::vec3(x, y, z);
}

Torus::Torus(float R, float r) : major_radius(R), minor_radius(r) {
    this->rho_range = {0.0f, 2.0f * M_PI};
    this->theta_range = {0.0f, 2.0f * M_PI};
    this->resolution = {32, 32};
    this->graph_func = &Torus::torusFunc;
    
    this->graph_var = Var(R, r);
}

void Torus::Init() {
    ThreeDObject::Init();
}
