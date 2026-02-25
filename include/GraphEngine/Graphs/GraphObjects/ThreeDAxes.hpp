#ifndef __THREEDAXES_HPP__
#define __THREEDAXES_HPP__

#include "GraphObject.hpp"
#include "../../Core/Shader.hpp"
#include "../../Core/GraphApp.hpp"
#include "../../Math/Vec3.hpp"
#include <vector>
#include <iostream>
#include "../../../glad.h"
#include "../../../glm/glm.hpp"
#include "../../../glm/gtc/matrix_transform.hpp"
#include "../../../Event/MouseEvents.hpp"
#include "../../../Event/KeyClicked.hpp"
#include "../Assets/Arrow.hpp"
#include <random>
#include "../GraphObjects/GraphMathObject.hpp"
#include "ThreeDObject.hpp"

class ThreeDAxes : public ThreeDObject
{
public:
    ThreeDAxes();
    void generate(Vec3 start, Vec3 end);

    void generatePoints() override;
    void add(glm::vec3 p)
    {
        points.push_back(p);
    }
};

#endif