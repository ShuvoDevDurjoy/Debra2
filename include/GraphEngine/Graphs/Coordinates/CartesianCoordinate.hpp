#ifndef __CARTESIANCOORDINATE_CPP__
#define __CARTESIANCOORDINATE_CPP__

#include <cmath>

#include "../GraphObject.hpp"
#include "../../Math/Vec2.hpp"
#include "../../Math/Vec3.hpp"
#include "../../Math/Var.hpp"
#include "../CartesianGraph.hpp"
#include "../../../glad.h"
#include "../../glm/glm.hpp"
#include <variant>
#include "../../glm/gtc/matrix_transform.hpp"
#include "../Assets/Arrow.hpp"

class CartesianCoordinate
{
    private:
        float t_start;
        float t_end;

        int resolution = 1000;

    public: 
        CartesianCoordinate(){}

        CartesianCoordinate(std::pair<float, float> t_range, int resolution){
            this->t_start = t_range.first;
            t_end = t_range.second;
            this->resolution = resolution;
        }

        void setResolution(int resolution){
            this->resolution = resolution;
        }

        void setRange(std::pair<float, float> t_range){
            t_start = t_range.first;
            t_end = t_range.second;
        }
        int getResolution() { return this->resolution; }

        CartesianGraph *generateFunction(Vec2 (*func)(float, Var), Var v);

        Arrow3D *drawArrow(Vec3 start, Vec3 end);
};

#endif