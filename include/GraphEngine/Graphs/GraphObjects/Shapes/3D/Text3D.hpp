#ifndef __TEXT3D_HPP__
#define __TEXT3D_HPP__

#include <GraphEngine/Graphs/GraphObjects/ThreeDObject.hpp>
#include <string>

class Char3D : public ThreeDObject {
public:
    char c;
    float x, y, z, scale;

public:
    Char3D(char c, float x, float y, float z, float scale);
    void generatePoints() override;

private:
    void generateBezierOutline(char c, float x, float y, float z, float scale);
};

class Text3D : public ThreeDObject {
public:
    std::string textStr;
    float x, y, z, scale;

public:
    Text3D(const std::string& textStr, float x, float y, float z, float scale);
    void generatePoints() override;
};

#endif
