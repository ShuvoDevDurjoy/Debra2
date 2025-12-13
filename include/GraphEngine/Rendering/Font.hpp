#ifndef __FONT_HPP__
#define __FONT_HPP__

#include <map>
#include <string>
#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "../include/glad.h"

class Font
{
public:
    struct Character
    {
        GLuint textureID;
        int sizeX, sizeY;
        int bearingX, bearingY;
        unsigned int advance;
    };

    static bool Load(const std::string &fontPath, int pixelSize);
    static void Render(const std::string &text, float x, float y, float scale,
                       float r, float g, float b);

    static GLuint VAO, VBO;
    static std::map<char, Character> Characters;

    static float projection[16]; // orthographic matrix
    static void UpdateProjection(int width, int height);
};

#endif
