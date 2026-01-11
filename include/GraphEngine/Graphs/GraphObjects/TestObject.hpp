#ifndef __TESTOBJECT_HPP__
#define __TESTOBJECT_HPP__

#include "GraphObject.hpp"

class TestObject: public GraphObject{
    public: 
        void setPoints(glm::vec3 p){
            points.push_back(p);
        }

        void setShader(std::string s1, std::string s2, std::string s3){
            geometricShaderPath = s2;
            vertexShaderPath = s1;
            fragmentShaderPath = s3;
        }
};

#endif