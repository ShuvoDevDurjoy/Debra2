#ifndef __LINE_HPP__
#define __LINE_HPP__

#include "../GraphObject.hpp"
#include "../../../glm/glm.hpp"
#include "../../../glm/gtc/matrix_transform.hpp"

class Line: public GraphObject{
    public:
        glm::vec3 from, to;

    public:
        Line(glm::vec3 from = glm::vec3(-10, -10,0), glm::vec3 to = glm::vec3(10, 10,0)){
            this->from = from;
            this->to = to;
            duration = 1.0f;
            setPoints(from);
            setPoints(to);
            setDimension(std::min(from.x, to.x), std::max(from.x, to.x), std::min(to.y, from.y), std::max(to.y, from.y));
        }
};

#endif