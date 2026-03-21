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
            Init();
        }

        void generatePoints() override
        {
            start_bezier_path(from);
            add_line_to(to);
        }
};

#endif