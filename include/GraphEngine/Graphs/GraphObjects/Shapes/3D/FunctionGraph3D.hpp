#ifndef __FUNCTIONGRAPH3D_HPP__
#define __FUNCTIONGRAPH3D_HPP__

#include <GraphEngine/Graphs/GraphObjects/ThreeDObject.hpp>
#include <functional>

class FunctionGraph3D : public ThreeDObject {
private:
    std::function<glm::vec3(float, float, Var)> func;
    Var m_var;
public:
    FunctionGraph3D(std::function<glm::vec3(float, float, Var)> f, 
                    Var v = Var(0.0f),
                    std::pair<float, float> rho_range = {-5.0f, 5.0f},
                    std::pair<float, float> theta_range = {-5.0f, 5.0f});
    
    void Init() override;
    void generatePoints() override;
};

#endif
