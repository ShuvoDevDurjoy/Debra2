#include <GraphEngine/Graphs/GraphObjects/Shapes/3D/FunctionGraph3D.hpp>

// No static func needed

FunctionGraph3D::FunctionGraph3D(std::function<glm::vec3(float, float, Var)> f, 
                                 Var v,
                                 std::pair<float, float> x_range,
                                 std::pair<float, float> y_range) 
    : func(f), m_var(v)
{
    this->rho_range = x_range;
    this->theta_range = y_range;
    this->resolution = {50, 50};
    this->graph_var = v;
}

void FunctionGraph3D::generatePoints() {
    points.clear();
    normals.clear();
    sub_surface_ranges.clear();
    
    float increment_rho = (rho_range.second - rho_range.first) / std::max(1.0f, resolution.first - 1.0f);
    float increment_theta = (theta_range.second - theta_range.first) / std::max(1.0f, resolution.second - 1.0f);
    
    for (int i = 0; i < resolution.first; ++i) {
        float rho = rho_range.first + i * increment_rho;
        for (int j = 0; j < resolution.second; ++j) {
            float theta = theta_range.first + j * increment_theta;
            
            glm::vec3 p = func ? func(rho, theta, graph_var) : glm::vec3(rho, theta, 0.0f);
            points.push_back(p);

            float eps = 0.001f;
            glm::vec3 p_rho = func ? func(rho + eps, theta, graph_var) : glm::vec3(rho + eps, theta, 0.0f);
            glm::vec3 p_theta = func ? func(rho, theta + eps, graph_var) : glm::vec3(rho, theta + eps, 0.0f);
            
            glm::vec3 dp_drho = (p_rho - p) / eps;
            glm::vec3 dp_dtheta = (p_theta - p) / eps;
            glm::vec3 n = glm::normalize(glm::cross(dp_drho, dp_dtheta));
            
            normals.push_back(n);
        }
    }
    sub_surface_ranges.clear();
    sub_surface_ranges.push_back({0, (int)resolution.first * (int)resolution.second});
}

void FunctionGraph3D::Init() {
    ThreeDObject::Init();
}
