#pragma once 

#include <vector>
#include <GraphEngine/Graphs/GraphObjects/GraphObject.hpp>


class Polygon: public GraphObject{
public: 
    std::vector<std::vector<glm::vec3>> p_points;

public: 
    Polygon(std::vector<std::vector<glm::vec3>>& p_points, bool make_smooth = false){
        this->p_points = p_points;
        if(make_smooth)
            makeSmooth();
        Init();
    }

    void generatePoints() override{
        for(auto &p_p: p_points){
            if(p_p.size() < 2) continue;
            start_bezier_path(p_p[0]);
            for (size_t i = 1; i < p_p.size(); ++i){
                add_line_to(p_p[i]);
            }
        }
    }

};