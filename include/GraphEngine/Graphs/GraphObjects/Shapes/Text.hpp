#ifndef __TEXT_HPP__
#define __TEXT_HPP__

#include "../GraphObject.hpp"
#include "../GraphGroup.hpp"
#include "../../../Rendering/Font.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

class Char : public GraphObject {
public:
    Char(char c, float x = 0, float y = 0, float scale = 0.05f);
    void generateBezierOutline(char c, float x, float y, float scale);
};

class Text : public GraphGroup {
public:
    Text(const std::string& text, float x = 0, float y = 0, float scale = 0.05f);
};

#endif
