#ifndef __TEXT_HPP__
#define __TEXT_HPP__

#include <ft2build.h>
#include <GraphEngine/Graphs/GraphObjects/GraphObject.hpp>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

#define FONT_FAMILY_PATH "./fonts/NunitoSans.ttf"

class Char : public GraphObject {
public:
    Char(char c, float x = 0, float y = 0, float scale = 0.05f);
    void generateBezierOutline(char c, float x, float y, float scale);
};

class Text : public GraphObject {
public:
    Text(const std::string& text, float x = 0, float y = 0, float scale = 0.05f);
};

#endif
