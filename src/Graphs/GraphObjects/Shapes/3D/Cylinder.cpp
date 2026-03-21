#include <GraphEngine/Graphs/GraphObjects/Shapes/3D/Cylinder.hpp>

glm::vec3 Cylinder::cylinderFunc(float z, float theta, Var var) {
    float r = var[0];
    float h = var[1];
    
    // z maps from 0 to height, but we center it: -h/2 to h/2
    float x = r * cos(theta);
    float y = z - (h / 2.0f);
    float z_pos = r * sin(theta);
    
    return glm::vec3(x, y, z_pos);
}

Cylinder::Cylinder(float r, float h) : radius(r), height(h) {
    this->rho_range = {0.0f, h};    // Defines the length mapping
    this->theta_range = {0.0f, 2.0f * M_PI};
    this->resolution = {10, 32};
    this->graph_func = &Cylinder::cylinderFunc;
    
    this->graph_var = Var(r, h);
}

void Cylinder::Init() {
    ThreeDObject::Init();
}
