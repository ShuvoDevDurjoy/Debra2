#ifndef __CUBE3D_HPP__
#define __CUBE3D_HPP__

#include <GraphEngine/Graphs/GraphObjects/ThreeDObject.hpp>

class Cube3D : public ThreeDObject {
private:
    float side_length;
public:
    Cube3D(float side_length = 2.0f);
    
    void generatePoints() override;
    void setFillData() override;
    void setStrokeData() override;
};

#endif
