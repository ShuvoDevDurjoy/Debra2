#pragma once

#include <GraphEngine/Graphs/GraphObjects/GraphObject.hpp>

class TestObject: public GraphObject{
    public: 
        TestObject(){
            use_bezier_always = true;
        }
        void setPoints(glm::vec3 p){
            if (points.empty()) {
                start_bezier_path(p);
            } else {
                add_line_to(p);
            }
            points.push_back(p);
            is_bezier_path = true;
        }
};
